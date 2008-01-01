
/* Copyright (c) 2007, Stefan Eilemann <eile@equalizergraphics.com> 
   All rights reserved. */

#include "channel.h"

#include "configEvent.h"

using namespace std;
using namespace eqBase;

#ifdef WIN32_API
#  define snprintf _snprintf
#endif

namespace eqPixelBench
{
struct EnumMap
{
    const char*    formatString;
    const char*    typeString;
    const uint32_t format;
    const uint32_t type;
};

#define ENUM_MAP_ITEM( format, type )          \
    { #format, #type, format, type }

static EnumMap enums[] = {
    ENUM_MAP_ITEM( GL_RGBA, GL_UNSIGNED_BYTE ), // initial buffer resize
    ENUM_MAP_ITEM( GL_RGBA, GL_UNSIGNED_BYTE ),
    ENUM_MAP_ITEM( GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV ),
    ENUM_MAP_ITEM( GL_BGRA, GL_UNSIGNED_BYTE ),
    ENUM_MAP_ITEM( GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV ),
    ENUM_MAP_ITEM( GL_RGB,  GL_UNSIGNED_BYTE ),
    ENUM_MAP_ITEM( GL_RGB,  GL_UNSIGNED_INT_8_8_8_8_REV),
    ENUM_MAP_ITEM( GL_BGR,  GL_UNSIGNED_BYTE ),
    ENUM_MAP_ITEM( GL_BGR,  GL_UNSIGNED_INT_8_8_8_8_REV ),
    ENUM_MAP_ITEM( GL_DEPTH_COMPONENT, GL_FLOAT ),
    ENUM_MAP_ITEM( GL_DEPTH_STENCIL_NV, GL_UNSIGNED_INT_24_8_NV ),
    { 0, 0, false, false }};

#define NUM_IMAGES 8

void Channel::frameDraw( const uint32_t frameID )
{
    //----- setup GL state
    applyBuffer();
    applyViewport();
            
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    applyFrustum();

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    applyHeadTransform();

    setupAssemblyState();

    //----- setup constant data
    eq::Frame*     frame     = new eq::Frame;
    eq::FrameData* frameData = new eq::FrameData;
    frame->setData( frameData );

    eq::Image*               image = frameData->newImage();
    const eq::PixelViewport& pvp    = getPixelViewport();
    const vmml::Vector2i     offset( pvp.x, pvp.y );
    eq::Config*              config = getConfig();

    ConfigEvent   event;
    const string& name  = getName();
    if( name.empty( ))    
        snprintf( event.data.user.data, 32, "%p", this);
    else
        snprintf( event.data.user.data, 32, "%s", name.c_str( ));
    event.data.user.data[31] = '\0';
    event.area.x = pvp.w;
    event.area.y = pvp.h;

    Clock          clock;

    //----- test all default format/type combinations
    glGetError();
    for( uint32_t i=0; enums[i].formatString; ++i )
    {
        // setup
        snprintf( event.formatType, 64, "%s/%s", 
                  enums[i].formatString, enums[i].typeString );
        event.formatType[63] = '\0';
        event.data.type = ConfigEvent::READBACK;

        image->setFormat( eq::Frame::BUFFER_COLOR, enums[i].format );
        image->setType(   eq::Frame::BUFFER_COLOR, enums[i].type );
        
        // read
        clock.reset();
        image->startReadback( eq::Frame::BUFFER_COLOR, pvp );
        image->syncReadback();
        event.msec = clock.getTimef();

        GLenum error = glGetError();
        if( error != GL_NO_ERROR )
            event.msec = - static_cast<float>( error );
        config->sendEvent( event );

        // draw
        event.data.type = ConfigEvent::ASSEMBLE;
        eq::Compositor::ImageOp op;
        op.channel = this;
        op.buffers = eq::Frame::BUFFER_COLOR;
        op.offset  = offset;

        clock.reset();
        eq::Compositor::assembleImage( image, op );
        event.msec = clock.getTimef();

        error = glGetError();
        if( error != GL_NO_ERROR )
            event.msec = - static_cast<float>( error );

        config->sendEvent( event );
    }

    //----- test tiled assembly algorithms
    frameData->clear();

    eq::PixelViewport subPVP = pvp;
    subPVP.h /= NUM_IMAGES;
    event.area.y = subPVP.h;

    for( unsigned i = 0; i < NUM_IMAGES; ++i )
    {
        subPVP.y = pvp.y + i * subPVP.h;

        // add another image to the frame, fill color image
        image = frameData->newImage();
        image->setFormat( eq::Frame::BUFFER_COLOR, GL_BGRA );
        image->setType(   eq::Frame::BUFFER_COLOR, GL_UNSIGNED_BYTE );
        
        image->startReadback( eq::Frame::BUFFER_COLOR, subPVP );
        image->syncReadback();

        // benchmark
        eq::Compositor::ImageOp op;
        op.channel = this;
        op.buffers = eq::Frame::BUFFER_COLOR | eq::Frame::BUFFER_DEPTH;
        op.offset  = offset;

        // fixed-function
        event.data.type = ConfigEvent::ASSEMBLE;
        snprintf( event.formatType, 64, 
                  "Tiled assembly (GL1.1) of %d images", i+1 ); 

        const vector< eq::Image* >& images = frame->getImages();
        clock.reset();
        for( vector< eq::Image* >::const_iterator j = images.begin(); 
             j != images.end(); ++j )
        
            eq::Compositor::assembleImage( *j, op );

        event.msec = clock.getTimef();
        config->sendEvent( event );            
    
        // CPU
        snprintf( event.formatType, 64,
                  "Tiled assembly (CPU)   of %d images", i+1 ); 

        std::vector< eq::Frame* > frames;
        frames.push_back( frame );

        clock.reset();
        eq::Compositor::assembleFramesCPU( frames, this );
        event.msec = clock.getTimef();
        config->sendEvent( event );            
    }

    //----- test depth-based assembly algorithms
    frameData->clear();
    event.area.y = pvp.h;

    for( unsigned i = 0; i < NUM_IMAGES; ++i )
    {
        // add another image to the frame, fill depth & color image
        image = frameData->newImage();
        image->setFormat( eq::Frame::BUFFER_COLOR, GL_BGRA );
        image->setType(   eq::Frame::BUFFER_COLOR, GL_UNSIGNED_BYTE );
        image->setFormat( eq::Frame::BUFFER_DEPTH, GL_DEPTH_COMPONENT );
        image->setType(   eq::Frame::BUFFER_DEPTH, GL_FLOAT );
        
        image->startReadback( eq::Frame::BUFFER_COLOR | 
                               eq::Frame::BUFFER_DEPTH, pvp );
        image->syncReadback();

        // benchmark
        eq::Compositor::ImageOp op;
        op.channel = this;
        op.buffers = eq::Frame::BUFFER_COLOR | eq::Frame::BUFFER_DEPTH;
        op.offset  = offset;

        // fixed-function
        event.data.type = ConfigEvent::ASSEMBLE;
        snprintf( event.formatType, 64, 
                  "Depth-based assembly (GL1.1) of %d images", i+1 ); 

        const vector< eq::Image* >& images = frame->getImages();
        clock.reset();
        for( vector< eq::Image* >::const_iterator j = images.begin(); 
             j != images.end(); ++j )
        
            eq::Compositor::assembleImageDB_FF( *j, op );

        event.msec = clock.getTimef();
        config->sendEvent( event );            
    
        // GLSL
        if( GLEW_VERSION_2_0 )
        {
            snprintf( event.formatType, 64,
                      "Depth-based assembly (GLSL)  of %d images", i+1 ); 
            clock.reset();
            for( vector< eq::Image* >::const_iterator j = images.begin(); 
                 j != images.end(); ++j )
        
                eq::Compositor::assembleImageDB_GLSL( *j, op );
            event.msec = clock.getTimef();
            config->sendEvent( event );
        }

        // CPU
        snprintf( event.formatType, 64, 
                  "Depth-based assembly (CPU)   of %d images", i+1 ); 

        std::vector< eq::Frame* > frames;
        frames.push_back( frame );

        clock.reset();
        eq::Compositor::assembleFramesCPU( frames, this );
        event.msec = clock.getTimef();
        config->sendEvent( event );            
    }
    
    resetAssemblyState();

    frame->setData( 0 );
    delete frameData;
    delete frame;
}
}
