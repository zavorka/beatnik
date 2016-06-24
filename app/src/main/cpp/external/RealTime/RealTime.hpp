#pragma once
#pragma GCC visibility push(default)

namespace reBass
{
    struct RealTime
    {
        int sec;
        int nsec;

        int usec() const { return nsec / 1000; }
        int msec() const { return nsec / 1000000; }

        RealTime(): sec(0), nsec(0) {}
        RealTime(int s, int n);

        RealTime(const RealTime &r) :
                sec(r.sec), nsec(r.nsec) { }

        static RealTime fromSeconds(double sec);

        RealTime &operator=(const RealTime &r) {
            sec = r.sec; nsec = r.nsec; return *this;
        }

        RealTime operator+(const RealTime &r) const {
            return RealTime(sec + r.sec, nsec + r.nsec);
        }
        RealTime operator-(const RealTime &r) const {
            return RealTime(sec - r.sec, nsec - r.nsec);
        }
        RealTime operator-() const {
            return RealTime(-sec, -nsec);
        }

        bool operator <(const RealTime &r) const {
            if (sec == r.sec) return nsec < r.nsec;
            else return sec < r.sec;
        }

        bool operator >(const RealTime &r) const {
            if (sec == r.sec) return nsec > r.nsec;
            else return sec > r.sec;
        }

        bool operator==(const RealTime &r) const {
            return (sec == r.sec && nsec == r.nsec);
        }

        bool operator!=(const RealTime &r) const {
            return !(r == *this);
        }

        bool operator>=(const RealTime &r) const {
            if (sec == r.sec) return nsec >= r.nsec;
            else return sec >= r.sec;
        }

        bool operator<=(const RealTime &r) const {
            if (sec == r.sec) return nsec <= r.nsec;
            else return sec <= r.sec;
        }

        /**
         * Convert a sample frame at the given sample rate into a RealTime.
         */
        static RealTime frame2RealTime(int frame, unsigned int sampleRate);

        static const RealTime zeroTime;
    };
}

#pragma GCC visibility pop
