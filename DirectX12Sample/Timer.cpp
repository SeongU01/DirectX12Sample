#include "pch.h"
#include "Timer.h"
Timer::Timer()
    : _secondsPerCount(0.0), _deltaTime(-1.0), _baseTime(0), _pausedTime(0),
    _prevTime(0), _currTime(0), _stopped(false)
{
    __int64 countsPerSec;
    QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
    _secondsPerCount = 1.0 / (double)countsPerSec;
}

// Returns the total time elapsed since Reset() was called, NOT counting any
// time when the clock is stopped.
float Timer::TotalTime() const
{
    // If we are stopped, do not count the time that has passed since we stopped.
    // Moreover, if we previously already had a pause, the distance
    // _stopTime - _baseTime includes paused time, which we do not want to count.
    // To correct this, we can subtract the paused time from _stopTime:
    //
    //                     |<--paused time-->|
    // ----*---------------*-----------------*------------*------------*------>
    // time
    //  _baseTime       _stopTime        startTime     _stopTime    _currTime

    if (_stopped)
    {
        return (float)(((_stopTime - _pausedTime) - _baseTime) * _secondsPerCount);
    }

    // The distance _currTime - _baseTime includes paused time,
    // which we do not want to count.  To correct this, we can subtract
    // the paused time from _currTime:
    //
    //  (_currTime - _pausedTime) - _baseTime
    //
    //                     |<--paused time-->|
    // ----*---------------*-----------------*------------*------> time
    //  _baseTime       _stopTime        startTime     _currTime

    else
    {
        return (float)(((_currTime - _pausedTime) - _baseTime) * _secondsPerCount);
    }
}

float Timer::DeltaTime() const
{
    return (float)_deltaTime;
}

void Timer::Reset()
{
    __int64 currTime;
    QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

    _baseTime = currTime;
    _prevTime = currTime;
    _stopTime = 0;
    _stopped = false;
}

void Timer::Start()
{
    __int64 startTime;
    QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

    // Accumulate the time elapsed between stop and start pairs.
    //
    //                     |<-------d------->|
    // ----*---------------*-----------------*------------> time
    //  _baseTime       _stopTime        startTime

    if (_stopped)
    {
        _pausedTime += (startTime - _stopTime);

        _prevTime = startTime;
        _stopTime = 0;
        _stopped = false;
    }
}

void Timer::Stop()
{
    if (!_stopped)
    {
        __int64 currTime;
        QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

        _stopTime = currTime;
        _stopped = true;
    }
}

void Timer::Tick()
{
    if (_stopped)
    {
        _deltaTime = 0.0;
        return;
    }

    __int64 currTime;
    QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
    _currTime = currTime;

    // Time difference between this frame and the previous.
    _deltaTime = (_currTime - _prevTime) * _secondsPerCount;

    // Prepare for next frame.
    _prevTime = _currTime;

    // Force nonnegative.  The DXSDK's CDXUTTimer mentions that if the
    // processor goes into a power save mode or we get shuffled to another
    // processor, then _deltaTime can be negative.
    if (_deltaTime < 0.0)
    {
        _deltaTime = 0.0;
    }
}