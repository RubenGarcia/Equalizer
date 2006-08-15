
/* Copyright (c) 2006, Stefan Eilemann <eile@equalizergraphics.com> 
   All rights reserved. */

#ifndef EQ_PLY_INITDATA_H
#define EQ_PLY_INITDATA_H

#include "eqPly.h"
#include "frameData.h"

#include <eq/eq.h>

class InitData : public eqNet::Object
{
public:
    InitData();
    InitData( const void* data, const uint64_t size );
    virtual ~InitData();

    void setFrameData( eqBase::RefPtr<FrameData> frameData );
    eqBase::RefPtr<FrameData> getFrameData();

    void               setFilename( const std::string& filename );
    void               setTrackerPort( const std::string& trackerPort );
    const std::string& getFilename() const { return _filename; }
    const std::string& getTrackerPort() const { return _trackerPort; }

protected:
    const void* getInstanceData( uint64_t* size );

private:
    uint32_t _frameDataID;
    eqBase::PerThread< FrameData* > _frameData;

    std::string _filename;
    std::string _trackerPort;

    char* _instanceData;
    void  _clearInstanceData();
};



#endif // EQ_PLY_INITDATA_H

