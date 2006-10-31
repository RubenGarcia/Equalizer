/* Copyright (c) 2006, Dustin Wueest <wueest@dustin.ch>
   All rights reserved. */

#ifndef FOB_TRACKER_H
#define FOB_TRACKER_H

#include <eq/eq.h>
#include <string>

class Tracker
{
public:
    /** 
     * Constructs a new Tracker and sets it's _running state to false.
     */
    Tracker();

    /**
     * Configures the serial port and initialises the tracker.
     *
     * Sets the _running state to true if the initialisation is successful.
     *
     * @param port the used serial port.
     * @return <code>true</code> if the tracker works correctly,
     *         <code>false</code> otherwise.
     */
    bool init( const std::string& port );

    /** 
     * Set the matrix defining the transformation from world coordinates to
     * emitter coordinates.
     *
     * @param matrix the world to emitter matrix.
     */
    void setWorldToEmitter( const vmml::Matrix4f& matrix )
        { _worldToEmitter = matrix; }

    /** 
     * Set the matrix defining the transformation from sensor coordinates to
     * coordinates of the tracked object.
     *
     * @param matrix the sensor to object matrix.
     */
    void setSensorToObject( const vmml::Matrix4f& matrix )
        { _sensorToObject = matrix; }

    /**
     * Checks if the tracker is running
     *
     * @return <code>true</code> if the tracker is ready,
     *         <code>false</code> otherwise.
     */
    bool isRunning() const { return _running; }

    /**
     * Gets new position and orientation data from the tracker and
     * stores them in the _pos and _hpr arrays.
     *
     * For a successful update, _running must be true.
     *
     * @return <code>true</code> if the data transfer is successful,
     *         <code>false</code> otherwise.
     */
    bool update();

    /**
     * Gets the transformation matrix with the position and orientation data.
     *
     * This function will not communicate with the tracker,.
     *
     * @return the transformation matrix.
     */
    const vmml::Matrix4f& getMatrix() const { return _matrix; }

private:
    bool _update(); //update without state checking
    bool _read( unsigned char* buffer, const size_t size,
                const unsigned long int timeout );

    /** The state defining if the tracker is running. */
    bool  _running;

    int   _fd;

    /** matrix defining the orientation and position of the tracked object. */
    vmml::Matrix4f _matrix;

    /** world to emitter transformation */
    vmml::Matrix4f _worldToEmitter;
    /** sensor to object transformation */
    vmml::Matrix4f _sensorToObject;


};

#endif // FOB_TRACKER_H
