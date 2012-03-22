
/* Copyright (c) 2005-2012, Stefan Eilemann <eile@equalizergraphics.com> 
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

#include "init.h"

#include "global.h"
#include "node.h"
#include "pluginRegistry.h"
#include "socketConnection.h"

#include <lunchbox/init.h>
#include <lunchbox/os.h>

#ifdef _MSC_VER
#  include <direct.h>
#  define getcwd _getcwd
#endif

#ifndef MAXPATHLEN
#  define MAXPATHLEN 1024
#endif

namespace co
{
namespace
{
    static co::base::a_int32_t _initialized;
}

bool _init( const int argc, char** argv )
{
    if( ++_initialized > 1 ) // not first
        return true;

    if( !base::init( argc, argv ))
        return false;

    // init all available plugins
    PluginRegistry& plugins = Global::getPluginRegistry();
#ifdef COLLAGE_DSO_NAME
    if( !plugins.addPlugin( COLLAGE_DSO_NAME ) && // Found by LDD
        // Hard-coded compile locations as backup:
        !plugins.addPlugin( std::string( EQ_BUILD_DIR ) + "lib/" + 
                            COLLAGE_DSO_NAME ) &&
#  ifdef NDEBUG
        !plugins.addPlugin( std::string( EQ_BUILD_DIR ) + "lib/Release/" +
                            COLLAGE_DSO_NAME )
#  else
        !plugins.addPlugin( std::string( EQ_BUILD_DIR ) + "lib/Debug/"
                            + COLLAGE_DSO_NAME )
#  endif
        )
    {
        EQWARN << "Built-in Collage plugins not loaded: " << COLLAGE_DSO_NAME
               << " not in library search path and hardcoded locations not "
               << "found" << std::endl;
    }

#else
#  ifndef NDEBUG
#    error "COLLAGE_DSO_NAME not defined"
#  endif
    EQWARN << "Built-in Collage plugins not loaded: COLLAGE_DSO_NAME not defined"
           << std::endl;
#endif
    plugins.init();

#ifdef _WIN32
    WORD    wsVersion = MAKEWORD( 2, 0 );
    WSADATA wsData;
    if( WSAStartup( wsVersion, &wsData ) != 0 )
    {
        EQERROR << "Initialization of Windows Sockets failed" 
                << base::sysError << std::endl;
        return false;
    }
#endif

    const std::string& programName = Global::getProgramName();
    if( programName.empty() && argc > 0 )
        Global::setProgramName( argv[0] );

    const std::string& workDir = Global::getWorkDir();
    if( workDir.empty( ))
    {
        char cwd[MAXPATHLEN];
        Global::setWorkDir( getcwd( cwd, MAXPATHLEN ));
    }
    return true;
}

bool exit()
{
    if( --_initialized > 0 ) // not last
        return true;
    EQASSERT( _initialized == 0 );

#ifdef _WIN32
    if( WSACleanup() != 0 )
    {
        EQERROR << "Cleanup of Windows Sockets failed" 
                << base::sysError << std::endl;
        return false;
    }
#endif

    // de-initialize registered plugins
    PluginRegistry& plugins = Global::getPluginRegistry();
    plugins.exit();

    return base::exit();
}

}
