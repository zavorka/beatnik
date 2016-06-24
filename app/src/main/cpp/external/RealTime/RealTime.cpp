#include "RealTime.hpp"

namespace reBass {

#define ONE_BILLION 1000000000

    RealTime::RealTime(int s, int n): sec(s), nsec(n)
    {
        if (sec == 0) {
            while (nsec <= -ONE_BILLION) { nsec += ONE_BILLION; --sec; }
            while (nsec >=  ONE_BILLION) { nsec -= ONE_BILLION; ++sec; }
        } else if (sec < 0) {
            while (nsec <= -ONE_BILLION) { nsec += ONE_BILLION; --sec; }
            while (nsec > 0)             { nsec -= ONE_BILLION; ++sec; }
        } else {
            while (nsec >=  ONE_BILLION) { nsec -= ONE_BILLION; ++sec; }
            while (nsec < 0)             { nsec += ONE_BILLION; --sec; }
        }
    }

    RealTime RealTime::fromSeconds(double sec)
    {
        if (sec >= 0) {
            return RealTime(int(sec), int((sec - int(sec)) * ONE_BILLION + 0.5));
        } else {
            return -fromSeconds(-sec);
        }
    }

    RealTime RealTime::frame2RealTime(int frame, unsigned int sampleRate)
    {
        if (frame < 0) return -frame2RealTime(-frame, sampleRate);

        RealTime rt;
        rt.sec = frame / int(sampleRate);
        frame -= rt.sec * long(sampleRate);
        rt.nsec = (int)(((double(frame) * 1000000.0) / sampleRate) * 1000.0);
        return rt;
    }

    const RealTime RealTime::zeroTime(0,0);

}

