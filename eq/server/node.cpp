
/* Copyright (c) 2005-2013, Stefan Eilemann <eile@equalizergraphics.com>
 *                    2011, Daniel Nachbaur <danielnachbaur@gmail.com>
 *                    2010, Cedric Stalder <cedric.stalder@gmail.com>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 2.1 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "node.h"

#include "channel.h"
#include "config.h"
#include "global.h"
#include "log.h"
#include "nodeFactory.h"
#include "pipe.h"
#include "server.h"
#include "window.h"

#include <eq/fabric/commands.h>
#include <eq/fabric/elementVisitor.h>
#include <eq/fabric/event.h>
#include <eq/fabric/paths.h>

#include <co/barrier.h>
#include <co/global.h>
#include <co/objectICommand.h>

#include <lunchbox/clock.h>
#include <lunchbox/launcher.h>
#include <lunchbox/os.h>
#include <lunchbox/sleep.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

namespace eq
{
namespace server
{
typedef fabric::Node< Config, Node, Pipe, NodeVisitor > Super;
typedef co::CommandFunc<Node> NodeFunc;
namespace
{
#define S_MAKE_ATTR_STRING( attr ) ( std::string("EQ_NODE_") + #attr )
std::string _sAttributeStrings[] = {
    S_MAKE_ATTR_STRING( SATTR_LAUNCH_COMMAND )
};
std::string _cAttributeStrings[] = {
    S_MAKE_ATTR_STRING( CATTR_LAUNCH_COMMAND_QUOTE )
};

void _addEnv( std::ostringstream& stream, const char* key )
{
    char* env = getenv( key );
    if( env )
        stream << key << "=" << env << " ";
}
}

Node::Node( Config* parent )
    : Super( parent )
    , _active( 0 )
    , _finishedFrame( 0 )
    , _flushedFrame( 0 )
    , _state( STATE_STOPPED )
    , _bufferedTasks( new co::BufferConnection )
    , _lastDrawPipe( 0 )
{
    const Global* global = Global::instance();
    for( int i=0; i < Node::SATTR_LAST; ++i )
    {
        const SAttribute attr = static_cast< SAttribute >( i );
        setSAttribute( attr, global->getNodeSAttribute( attr ));
    }
    for( int i=0; i < Node::CATTR_LAST; ++i )
    {
        const CAttribute attr = static_cast< CAttribute >( i );
        setCAttribute( attr, global->getNodeCAttribute( attr ));
    }
    for( int i = 0; i < IATTR_LAST; ++i )
    {
        const IAttribute attr = static_cast< IAttribute >( i );
        setIAttribute( attr, global->getNodeIAttribute( attr ));
    }
}

Node::~Node()
{
}

void Node::attach( const uint128_t& id, const uint32_t instanceID )
{
    Super::attach( id, instanceID );

    co::CommandQueue* cmdQ = getCommandThreadQueue();
    registerCommand( fabric::CMD_OBJECT_SYNC,
                     NodeFunc( this, &Node::_cmdSync ), cmdQ );
    registerCommand( fabric::CMD_NODE_CONFIG_INIT_REPLY,
                     NodeFunc( this, &Node::_cmdConfigInitReply ), cmdQ );
    registerCommand( fabric::CMD_NODE_CONFIG_EXIT_REPLY,
                     NodeFunc( this, &Node::_cmdConfigExitReply ), cmdQ );
    registerCommand( fabric::CMD_NODE_FRAME_FINISH_REPLY,
                     NodeFunc( this, &Node::_cmdFrameFinishReply ), cmdQ );
}

ServerPtr Node::getServer()
{
    return getConfig() ? getConfig()->getServer() : 0;
}

ConstServerPtr Node::getServer() const
{
    return getConfig() ? getConfig()->getServer() : 0;
}

co::CommandQueue* Node::getMainThreadQueue()
{
    return getConfig()->getMainThreadQueue();
}

co::CommandQueue* Node::getCommandThreadQueue()
{
    return getConfig()->getCommandThreadQueue();
}

Channel* Node::getChannel( const ChannelPath& path )
{
    const Pipes& pipes = getPipes();
    LBASSERT( pipes.size() > path.pipeIndex );

    if( pipes.size() <= path.pipeIndex )
        return 0;

    return pipes[ path.pipeIndex ]->getChannel( path );
}

void Node::addTasks( const uint32_t tasks )
{
    setTasks( getTasks() | tasks );
}

void Node::activate()
{
    ++_active;
    LBLOG( LOG_VIEW ) << "activate: " << _active << std::endl;
}

void Node::deactivate()
{
    LBASSERT( _active != 0 );
    --_active;
    LBLOG( LOG_VIEW ) << "deactivate: " << _active << std::endl;
};

void Node::setSAttribute( const SAttribute attr, const std::string& value )
{
    _sAttributes[attr] = value;
}

const std::string& Node::getSAttribute( const SAttribute attr ) const
{
    return _sAttributes[attr];
}

const std::string& Node::getSAttributeString( const SAttribute attr )
{
    return _sAttributeStrings[attr];
}

void Node::setCAttribute( const CAttribute attr, const char value )
{
    _cAttributes[attr] = value;
}

char Node::getCAttribute( const CAttribute attr ) const
{
    return _cAttributes[attr];
}

const std::string& Node::getCAttributeString( const CAttribute attr )
{
    return _cAttributeStrings[attr];
}

//===========================================================================
// Operations
//===========================================================================

//---------------------------------------------------------------------------
// launch and connect
//---------------------------------------------------------------------------

namespace
{
static co::NodePtr _createNetNode( Node* node )
{
    co::NodePtr netNode = new co::Node;
    const co::ConnectionDescriptions& descriptions =
        node->getConnectionDescriptions();
    for( co::ConnectionDescriptionsCIter i = descriptions.begin();
         i != descriptions.end(); ++i )
    {
        netNode->addConnectionDescription( new co::ConnectionDescription( **i));
    }

    return netNode;
}
}

bool Node::connect()
{
    LBASSERT( isActive( ));

    if( _node.isValid( ))
        return _node->isConnected();

    if( !isStopped( ))
    {
        LBASSERT( _state == STATE_FAILED );
        return true;
    }

    co::LocalNodePtr localNode = getLocalNode();
    LBASSERT( localNode.isValid( ));

    if( !_node )
    {
        LBASSERT( !isApplicationNode( ));
        _node = _createNetNode( this );
    }
    else
    {
        LBASSERT( isApplicationNode( ));
    }

    LBLOG( LOG_INIT ) << "Connecting node" << std::endl;
    if( !localNode->connect( _node ) && !launch( ))
    {
        LBWARN << "Connection to " << _node->getNodeID() << " failed"
               << std::endl;
        _state = STATE_FAILED;
        _node = 0;
        return false;
    }

    return true;
}

bool Node::launch()
{
    for( co::ConnectionDescriptionsCIter i = _connectionDescriptions.begin();
         _host.empty() && i != _connectionDescriptions.end(); ++i )
    {
        _host = (*i)->getHostname();
        LBWARN << "No host specified, guessing " << _host << " from " << *i
               << std::endl;
    }

    if( _launch( _host ))
        return true;

    // Equalizer 1.0 style: try hostnames from connection descriptions
    for( co::ConnectionDescriptionsCIter i = _connectionDescriptions.begin();
         i != _connectionDescriptions.end(); ++i )
    {
        const std::string& hostname = (*i)->getHostname();
        if( hostname != _host && _launch( hostname ))
            return true;
    }

    sendError( fabric::ERROR_NODE_LAUNCH ) << _host;
    return false;
}

bool Node::syncLaunch( const lunchbox::Clock& clock )
{
    LBASSERT( isActive( ));

    if( !_node )
        return false;

    if( _node->isConnected( ))
        return true;

    LBASSERT( !isApplicationNode( ));
    co::LocalNodePtr localNode = getLocalNode();
    LBASSERT( localNode.isValid( ));

    const int32_t timeOut = getIAttribute( IATTR_LAUNCH_TIMEOUT );

    while( true )
    {
        co::NodePtr node = localNode->getNode( _node->getNodeID( ));
        if( node && node->isConnected( ))
        {
            LBASSERT( _node->getRefCount() == 1 );
            _node = node; // Use co::Node already connected
            return true;
        }

        lunchbox::sleep( 100 /*ms*/ );
        if( timeOut != static_cast<int32_t>(LB_TIMEOUT_INDEFINITE) &&
            clock.getTime64() > timeOut )
        {
            LBASSERT( _node->getRefCount() == 1 );
            _node = 0;
            std::ostringstream data;

            for( co::ConnectionDescriptions::const_iterator i =
                     _connectionDescriptions.begin();
                 i != _connectionDescriptions.end(); ++i )
            {
                co::ConnectionDescriptionPtr desc = *i;
                data << desc->getHostname() << ' ';
            }

            sendError( fabric::ERROR_NODE_CONNECT ) << _host;
            _state = STATE_FAILED;
            return false;
        }
    }
}

bool Node::_launch( const std::string& hostname ) const
{
    const std::string& command = getSAttribute( SATTR_LAUNCH_COMMAND );
    const size_t commandLen = command.size();

    bool commandFound = false;
    size_t lastPos = 0;
    std::string cmd;

    for( size_t percentPos = command.find( '%' );
         percentPos != std::string::npos;
         percentPos = command.find( '%', percentPos+1 ))
    {
        std::ostringstream replacement;
        switch( command[percentPos+1] )
        {
            case 'c':
            {
                replacement << _createRemoteCommand();
                commandFound = true;
                break;
            }
            case 'h':
            {
                if( hostname.empty( ))
                    replacement << "127.0.0.1";
                else
                    replacement << hostname;
                break;
            }
            case 'n':
                replacement << _node->getNodeID();
                break;

            case 'd':
                replacement << getConfig()->getWorkDir();
                break;

            case 'q':
                replacement << getCAttribute( CATTR_LAUNCH_COMMAND_QUOTE );
                break;

            default:
                LBWARN << "Unknown token " << command[percentPos+1]
                       << std::endl;
                replacement << '%' << command[percentPos+1];
        }

        cmd += command.substr( lastPos, percentPos-lastPos );
        if( !replacement.str().empty( ))
            cmd += replacement.str();

        lastPos  = percentPos+2;
    }

    cmd += command.substr( lastPos, commandLen-lastPos );

    if( !commandFound )
        cmd += " " + _createRemoteCommand();

    LBVERB << "Launch command: " << cmd << std::endl;
    if( lunchbox::Launcher::run( cmd ))
        return true;

    LBWARN << "Could not launch node using '" << cmd << "'" << std::endl;
    return false;
}

std::string Node::_createRemoteCommand() const
{
    const Config* config = getConfig();
    std::string program = config->getRenderClient();
    if( program.empty( ))
    {
        LBWARN << "No render client name, auto-launch will fail" << std::endl;
        return program;
    }

    //----- environment
    std::ostringstream stringStream;
    const char quote = getCAttribute( CATTR_LAUNCH_COMMAND_QUOTE );

    //----- program + args
#ifndef WIN32
#  ifdef Darwin
    const char libPath[] = "DYLD_LIBRARY_PATH";
#  else
    const char libPath[] = "LD_LIBRARY_PATH";
#  endif

    stringStream << "env "; // XXX
    _addEnv( stringStream, libPath );
    _addEnv( stringStream, "PATH" );
    _addEnv( stringStream, "PYTHONPATH" );

    for( int i=0; environ[i] != 0; ++i )
    {
        if( strlen( environ[i] ) > 2 &&
            ( strncmp( environ[i], "LB_", 3 ) == 0 ||
              strncmp( environ[i], "CO_", 3 ) == 0 ||
              strncmp( environ[i], "EQ_", 3 ) == 0 ))
        {
            stringStream << quote << environ[i] << quote << " ";
        }
    }

    stringStream << "LB_LOG_LEVEL=" <<lunchbox::Log::getLogLevelString() << " ";
    if( lunchbox::Log::topics != 0 )
        stringStream << "LB_LOG_TOPICS=" <<lunchbox::Log::topics << " ";

#endif // WIN32
    const boost::filesystem::path absolute =
        boost::filesystem::system_complete( boost::filesystem::path( program ));
    program = absolute.string();

    const std::string& ownData = getServer()->serialize();
    const std::string& remoteData = _node->serialize();
    const std::string& workDir = config->getWorkDir();
    std::string collageGlobals;
    co::Global::toString( collageGlobals );

    stringStream
        << quote << program << quote << " -- --eq-client " << quote
        << remoteData << workDir << CO_SEPARATOR << ownData << quote
        << " --co-globals " << quote << collageGlobals << quote;

    return stringStream.str();
}

//---------------------------------------------------------------------------
// init
//---------------------------------------------------------------------------
void Node::configInit( const uint128_t& initID, const uint32_t frameNumber )
{
    LBASSERT( _state == STATE_STOPPED );
    _state = STATE_INITIALIZING;

    const Config* config = getConfig();
    _flushedFrame  = config->getFinishedFrame();
    _finishedFrame = config->getFinishedFrame();
    _frameIDs.clear();

    LBLOG( LOG_INIT ) << "Create node" << std::endl;
    getConfig()->send( _node, fabric::CMD_CONFIG_CREATE_NODE ) << getID();

    LBLOG( LOG_INIT ) << "Init node" << std::endl;
    send( fabric::CMD_NODE_CONFIG_INIT ) << initID << frameNumber;
}

bool Node::syncConfigInit()
{
    LBASSERT( _state == STATE_INITIALIZING || _state == STATE_INIT_SUCCESS ||
              _state == STATE_INIT_FAILED );

    _state.waitNE( STATE_INITIALIZING );

    if( _state == STATE_INIT_SUCCESS )
    {
        _state = STATE_RUNNING;
        return true;
    }

    LBWARN << "Node initialization failed" << std::endl;
    configExit();
    return false;
}

//---------------------------------------------------------------------------
// exit
//---------------------------------------------------------------------------
void Node::configExit()
{
    if( _state == STATE_EXITING )
        return;

    LBASSERT( _state == STATE_RUNNING || _state == STATE_INIT_FAILED );
    _state = STATE_EXITING;

    LBLOG( LOG_INIT ) << "Exit node" << std::endl;
    send( fabric::CMD_NODE_CONFIG_EXIT );
    flushSendBuffer();
}

bool Node::syncConfigExit()
{
    LBASSERT( _state == STATE_EXITING || _state == STATE_EXIT_SUCCESS ||
              _state == STATE_EXIT_FAILED );

    _state.waitNE( STATE_EXITING );
    const bool success = ( _state == STATE_EXIT_SUCCESS );
    LBASSERT( success || _state == STATE_EXIT_FAILED );

    _state = isActive() ? STATE_FAILED : STATE_STOPPED;
    setTasks( fabric::TASK_NONE );
    _frameIDs.clear();
    _flushBarriers();
    return success;
}

//---------------------------------------------------------------------------
// update
//---------------------------------------------------------------------------
void Node::update( const uint128_t& frameID, const uint32_t frameNumber )
{
    if( !isRunning( ))
        return;

    LBVERB << "Start frame " << frameNumber << std::endl;
    LBASSERT( isActive( ));

    _frameIDs[ frameNumber ] = frameID;

    uint128_t configVersion = co::VERSION_INVALID;
    if( !isApplicationNode( )) // synced in Config::_cmdFrameStart
        configVersion = getConfig()->getVersion();

    send( fabric::CMD_NODE_FRAME_START )
            << getVersion() << configVersion << frameID << frameNumber;
    LBLOG( LOG_TASKS ) << "TASK node start frame " << std::endl;

    const Pipes& pipes = getPipes();
    for( Pipes::const_iterator i = pipes.begin(); i != pipes.end(); ++i )
        (*i)->update( frameID, frameNumber );

    if( !_lastDrawPipe ) // no FrameDrawFinish sent
    {
        send( fabric::CMD_NODE_FRAME_DRAW_FINISH ) << frameID << frameNumber;
        LBLOG( LOG_TASKS ) << "TASK node draw finish " << getName() <<  " "
                           << std::endl;
    }
    _lastDrawPipe = 0;

    send( fabric::CMD_NODE_FRAME_TASKS_FINISH ) << frameID << frameNumber;
    LBLOG( LOG_TASKS ) << "TASK node tasks finish " << std::endl;

    _finish( frameNumber );
    flushSendBuffer();
}

uint32_t Node::_getFinishLatency() const
{
    switch( getIAttribute( Node::IATTR_THREAD_MODEL ))
    {
        case fabric::UNDEFINED:
        case fabric::DRAW_SYNC:
            if( getTasks() & fabric::TASK_DRAW )
            {
                // More than one frame latency doesn't make sense, since the
                // draw sync for frame+1 does not allow for more
                const Config* config = getConfig();
                const uint32_t latency = config->getLatency();

                return LB_MIN( latency, 1 );
            }
            break;

        case fabric::LOCAL_SYNC:
            if( getTasks() != fabric::TASK_NONE )
                // local sync enforces no latency
                return 0;
            break;

        case fabric::ASYNC:
            break;
        default:
            LBUNIMPLEMENTED;
    }

    const Config* config = getConfig();
    return config->getLatency();
}

void Node::_finish( const uint32_t currentFrame )
{
    const Pipes& pipes = getPipes();
    for( Pipes::const_iterator i = pipes.begin(); i != pipes.end(); ++i )
    {
        const Pipe* pipe = *i;
        if( pipe->getIAttribute( Pipe::IATTR_HINT_THREAD ) && pipe->isRunning())
        {
            const uint32_t latency = _getFinishLatency();
            if( currentFrame > latency )
                flushFrames( currentFrame - latency );
            return;
        }
    }

    // else only non-threaded pipes, all local tasks are done, send finish now.
    flushFrames( currentFrame );
}

void Node::flushFrames( const uint32_t frameNumber )
{
    LBLOG( LOG_TASKS ) << "Flush frames including " << frameNumber << std::endl;

    while( _flushedFrame < frameNumber )
    {
        ++_flushedFrame;
        _sendFrameFinish( _flushedFrame );
    }

    flushSendBuffer();
}

void Node::_sendFrameFinish( const uint32_t frameNumber )
{
    FrameIDHash::iterator i = _frameIDs.find( frameNumber );
    if( i == _frameIDs.end( ))
        return; // finish already send

    send( fabric::CMD_NODE_FRAME_FINISH ) << i->second << frameNumber;
    _frameIDs.erase( i );
    LBLOG( LOG_TASKS ) << "TASK node finish frame " << frameNumber << std::endl;
}

//---------------------------------------------------------------------------
// Barrier cache
//---------------------------------------------------------------------------
co::Barrier* Node::getBarrier()
{
    if( _barriers.empty( ))
    {
        co::Barrier* barrier = new co::Barrier( getServer(),
                                                _node->getNodeID( ));
        barrier->setAutoObsolete( getConfig()->getLatency() + 1 );
        return barrier;
    }
    // else

    co::Barrier* barrier = _barriers.back();
    _barriers.pop_back();
    barrier->setHeight( 0 );
    return barrier;
}

void Node::changeLatency( const uint32_t latency )
{
    for( co::Barriers::const_iterator i = _barriers.begin();
         i != _barriers.end(); ++ i )
    {
        co::Barrier* barrier = *i;
        barrier->setAutoObsolete( latency + 1 );
    }
}

void Node::releaseBarrier( co::Barrier* barrier )
{
    _barriers.push_back( barrier );
}

void Node::_flushBarriers()
{
    for( co::BarriersCIter i =_barriers.begin(); i != _barriers.end(); ++i )
        delete *i;
    _barriers.clear();
}

bool Node::removeConnectionDescription( co::ConnectionDescriptionPtr cd )
{
    // Don't use std::find, RefPtr::operator== compares pointers, not values.
    for(co::ConnectionDescriptions::iterator i=_connectionDescriptions.begin();
        i != _connectionDescriptions.end(); ++i )
    {
        if( *cd != **i )
            continue;

        _connectionDescriptions.erase( i );
        return true;
    }
    return false;
}

co::ObjectOCommand Node::send( const uint32_t cmd )
{
    return send( cmd, getID( ));
}

co::ObjectOCommand Node::send( const uint32_t cmd, const uint128_t& id )
{
    return co::ObjectOCommand( co::Connections( 1, _bufferedTasks ), cmd,
                               co::COMMANDTYPE_OBJECT, id, CO_INSTANCE_ALL );
}

EventOCommand Node::sendError( const uint32_t error )
{
    return getConfig()->sendError( Event::NODE_ERROR, Error( error, getID( )));
}

void Node::flushSendBuffer()
{
    _bufferedTasks->sendBuffer( _node->getConnection( ));
}

//===========================================================================
// command handling
//===========================================================================
bool Node::_cmdConfigInitReply( co::ICommand& cmd )
{
    co::ObjectICommand command( cmd );
    LBVERB << "handle configInit reply " << command << std::endl;
    LBASSERT( _state == STATE_INITIALIZING );
    _state = command.read< uint64_t >() ? STATE_INIT_SUCCESS : STATE_INIT_FAILED;

    return true;
}

bool Node::_cmdConfigExitReply( co::ICommand& cmd )
{
    co::ObjectICommand command( cmd );
    LBVERB << "handle configExit reply " << command << std::endl;
    LBASSERT( _state == STATE_EXITING );

    _state = command.read< bool >() ? STATE_EXIT_SUCCESS : STATE_EXIT_FAILED;
    return true;
}

bool Node::_cmdFrameFinishReply( co::ICommand& cmd )
{
    co::ObjectICommand command( cmd );
    LBVERB << "handle frame finish reply " << command << std::endl;

    const uint32_t frameNumber = command.read< uint32_t >();

    _finishedFrame = frameNumber;
    getConfig()->notifyNodeFrameFinished( frameNumber );

    return true;
}

void Node::output( std::ostream& os ) const
{
    if( !_host.empty( ))
        os << "host     \"" << _host << '"' << std::endl;

    const co::ConnectionDescriptions& descriptions = _connectionDescriptions;
    for( co::ConnectionDescriptions::const_iterator i = descriptions.begin();
         i != descriptions.end(); ++i )
    {
        co::ConnectionDescriptionPtr desc = *i;
        os << *desc;
    }

    bool attrPrinted   = false;

    for( Node::SAttribute i = static_cast<Node::SAttribute>( 0 );
         i < Node::SATTR_LAST;
         i = static_cast<Node::SAttribute>( static_cast<uint32_t>( i )+1))
    {
        const std::string& value = getSAttribute( i );
        if( value == Global::instance()->getNodeSAttribute( i ))
            continue;

        if( !attrPrinted )
        {
            os << std::endl << "attributes" << std::endl;
            os << "{" << std::endl << lunchbox::indent;
            attrPrinted = true;
        }

        os << ( i==Node::SATTR_LAUNCH_COMMAND ? "launch_command       " :
                "ERROR" )
           << "\"" << value << "\"" << std::endl;
    }

    for( Node::CAttribute i = static_cast<Node::CAttribute>( 0 );
         i < Node::CATTR_LAST;
         i = static_cast<Node::CAttribute>( static_cast<uint32_t>( i )+1))
    {
        const char value = getCAttribute( i );
        if( value == Global::instance()->getNodeCAttribute( i ))
            continue;

        if( !attrPrinted )
        {
            os << std::endl << "attributes" << std::endl;
            os << "{" << std::endl << lunchbox::indent;
            attrPrinted = true;
        }

        os << ( i==Node::CATTR_LAUNCH_COMMAND_QUOTE ? "launch_command_quote " :
                "ERROR" )
           << "'" << value << "'" << std::endl;
    }

    for( Node::IAttribute i = static_cast< Node::IAttribute>( 0 );
         i < Node::IATTR_LAST;
         i = static_cast< Node::IAttribute >( static_cast<uint32_t>( i )+1))
    {
        const int value = getIAttribute( i );
        if( value == Global::instance()->getNodeIAttribute( i ))
            continue;

        if( !attrPrinted )
        {
            os << std::endl << "attributes" << std::endl;
            os << "{" << std::endl << lunchbox::indent;
            attrPrinted = true;
        }

        os << ( i== Node::IATTR_LAUNCH_TIMEOUT ? "launch_timeout       " :
                i== Node::IATTR_THREAD_MODEL   ? "thread_model         " :
                i== Node::IATTR_HINT_AFFINITY  ? "hint_affinity        " :
                "ERROR" )
           << static_cast< fabric::IAttribute >( value ) << std::endl;
    }

    if( attrPrinted )
        os << lunchbox::exdent << "}" << std::endl << std::endl;
}

}
}

#include "../fabric/node.ipp"
template class eq::fabric::Node< eq::server::Config, eq::server::Node,
                                 eq::server::Pipe, eq::server::NodeVisitor >;
/** @cond IGNORE */
template std::ostream& eq::fabric::operator << ( std::ostream&,
                                                 const eq::server::Super& );
/** @endcond */
