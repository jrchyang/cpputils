#pragma once

#include <math.h>
#include <time.h>
#include <string.h>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <linux/types.h>
#include "namespace.h"

namespace TOPNSPC {

typedef std::chrono::duration<uint64_t, std::nano> timespan;

inline __u32 cap_to_u32_max(__u64 t) {
  return std::min(t, (__u64)std::numeric_limits<uint32_t>::max());
}

class utime_t {
public:
	struct {
		__u32 tv_sec;
		__u32 tv_nsec;
	} tv;

	bool is_zero() const {
		return (tv.tv_sec == 0) && (tv.tv_nsec == 0);
	}
	void normalize() {
		if (tv.tv_nsec > 1000000000ul) {
			tv.tv_sec = cap_to_u32_max(tv.tv_sec + tv.tv_nsec / (1000000000ul));
			tv.tv_nsec %= 1000000000ul;
		}
	}

	utime_t() {
		tv.tv_sec = 0;
		tv.tv_nsec = 0;
	}
	utime_t(time_t s, int n) {
		tv.tv_sec = s;
		tv.tv_nsec = n;
		normalize();
	}
	utime_t(const struct timespec v) {
		tv.tv_sec = v.tv_sec;
		tv.tv_nsec = v.tv_nsec;
	}
	utime_t(const struct timeval &v) {
		set_from_timeval(&v);
	}
	utime_t(const struct timeval *v) {
		set_from_timeval(v);
	}

	void to_timespec(struct timespec *ts) const {
		ts->tv_sec = tv.tv_sec;
		ts->tv_nsec = tv.tv_nsec;
	}
	void set_from_double(double d) {
		tv.tv_sec = (__u32)trunc(d);
		tv.tv_nsec = (__u32)((d - (double)tv.tv_sec) * 1000000000.0);
	}

	time_t sec() const { return tv.tv_sec; }
	long  usec() const { return tv.tv_nsec / 1000; }
	int   nsec() const { return tv.tv_nsec; }

	__u32& nsec_ref() { return tv.tv_nsec; }
	__u32&  sec_ref() { return tv.tv_sec; }

	uint64_t to_nsec() const {
		return (uint64_t)tv.tv_nsec + (uint64_t)tv.tv_sec*1000ull;
	}
	uint64_t to_msec() const {
		return (uint64_t)tv.tv_nsec/1000000ull + (uint64_t)tv.tv_sec*1000ull;
	}

	void copy_to_timeval(struct timeval *v) const {
		v->tv_sec = tv.tv_sec;
		v->tv_usec = tv.tv_nsec/1000;
	}
	void set_from_timeval(const struct timeval *v) {
		tv.tv_sec = v->tv_sec;
		tv.tv_nsec = v->tv_usec*1000;
	}

	void padding_check() {
		static_assert(sizeof(utime_t) == sizeof(tv.tv_sec)+sizeof(tv.tv_nsec), "utime_t have padding");
	}

	utime_t round_to_minute() {
		struct tm bdt;
		time_t tt = sec();
		localtime_r(&tt, &bdt);
		bdt.tm_sec = 0;
		tt = mktime(&bdt);
		return utime_t(tt, 0);
	}
	utime_t round_to_hour() {
		struct tm bdt;
		time_t tt = sec();
		localtime_r(&tt, &bdt);
		bdt.tm_sec = 0;
		bdt.tm_min = 0;
		tt = mktime(&bdt);
		return utime_t(tt, 0);
	}
	utime_t round_to_day() {
		struct tm bdt;
		time_t tt = sec();
		localtime_r(&tt, &bdt);
		bdt.tm_sec = 0;
		bdt.tm_min = 0;
		bdt.tm_hour = 0;
		tt = mktime(&bdt);
		return utime_t(tt, 0);
	}

	// 转换为 double
	operator double() const {
		return (double)sec() + ((double)nsec() / 1000000000.0f);
	}

	// 输出
	std::ostream& gmtime(std::ostream& out, bool legacy_form=false) const {
		out.setf(std::ios::right);
		char oldfill = out.fill();
		out.fill('0');

		if (sec() < ((time_t)(60*60*24*365*10))) {
			// raw seconds.  this looks like a relative time.
			out << (long)sec() << "." << std::setw(6) << usec();
		} else {
			// this looks like an absolute time.
			struct tm bdt;
			time_t tt = sec();
			gmtime_r(&tt, &bdt);
			out << std::setw(4) << (bdt.tm_year+1900)  // 2007 -> '07'
			    << '-' << std::setw(2) << (bdt.tm_mon+1)
			    << '-' << std::setw(2) << bdt.tm_mday;
			if (legacy_form) {
				out << ' ';
			} else {
				out << 'T';
			}
			out << std::setw(2) << bdt.tm_hour
			    << ':' << std::setw(2) << bdt.tm_min
			    << ':' << std::setw(2) << bdt.tm_sec;
			out << "." << std::setw(6) << usec();
			out << "Z";
		}
		out.fill(oldfill);
		out.unsetf(std::ios::right);
		return out;
	}
	std::ostream& gmtime_nsec(std::ostream& out) const {
		out.setf(std::ios::right);
		char oldfill = out.fill();
		out.fill('0');

		if (sec() < ((time_t)(60*60*24*365*10))) {
			// raw seconds.  this looks like a relative time.
			out << (long)sec() << "." << std::setw(6) << usec();
		} else {
			// this looks like an absolute time.
			struct tm bdt;
			time_t tt = sec();
			gmtime_r(&tt, &bdt);
			out << std::setw(4) << (bdt.tm_year+1900)  // 2007 -> '07'
			    << '-' << std::setw(2) << (bdt.tm_mon+1)
			    << '-' << std::setw(2) << bdt.tm_mday
			    << 'T'
			    << std::setw(2) << bdt.tm_hour
			    << ':' << std::setw(2) << bdt.tm_min
			    << ':' << std::setw(2) << bdt.tm_sec;
			out << "." << std::setw(9) << nsec();
			out << "Z";
		}
		out.fill(oldfill);
		out.unsetf(std::ios::right);
		return out;
	}
	std::ostream& asctime(std::ostream& out) const {
		out.setf(std::ios::right);
		char oldfill = out.fill();
		out.fill('0');

		if (sec() < ((time_t)(60*60*24*365*10))) {
			// raw seconds.  this looks like a relative time.
			out << (long)sec() << "." << std::setw(6) << usec();
		} else {
			// this looks like an absolute time.
			struct tm bdt;
			time_t tt = sec();
			gmtime_r(&tt, &bdt);

			char buf[128];
			asctime_r(&bdt, buf);
			int len = strlen(buf);
			if (buf[len - 1] == '\n')
				buf[len - 1] = '\0';
			out << buf;
		}
		out.fill(oldfill);
		out.unsetf(std::ios::right);
		return out;
	}
	std::ostream& localtime(std::ostream& out, bool legacy_form=false) const {
		out.setf(std::ios::right);
		char oldfill = out.fill();
		out.fill('0');

		if (sec() < ((time_t)(60*60*24*365*10))) {
			// raw seconds.  this looks like a relative time.
			out << (long)sec() << "." << std::setw(6) << usec();
		} else {
			// this looks like an absolute time.
			struct tm bdt;
			time_t tt = sec();
			localtime_r(&tt, &bdt);
			out << std::setw(4) << (bdt.tm_year+1900)  // 2007 -> '07'
			    << '-' << std::setw(2) << (bdt.tm_mon+1)
			    << '-' << std::setw(2) << bdt.tm_mday;
			if (legacy_form) {
				out << ' ';
			} else {
				out << 'T';
			}
			out << std::setw(2) << bdt.tm_hour
			    << ':' << std::setw(2) << bdt.tm_min
			    << ':' << std::setw(2) << bdt.tm_sec;
			out << "." << std::setw(6) << usec();
			if (!legacy_form) {
				char buf[32] = { 0 };
				strftime(buf, sizeof(buf), "%z", &bdt);
				out << buf;
			}
		}
		out.fill(oldfill);
		out.unsetf(std::ios::right);
		return out;
	}
};

// 运算符重载
inline utime_t operator+(const utime_t& l, const utime_t& r) {
	__u64 sec = (__u64)l.sec() + r.sec();
	return utime_t(cap_to_u32_max(sec), l.nsec() + r.nsec());
}
inline utime_t& operator+=(utime_t& l, const utime_t& r) {
	l.sec_ref() = cap_to_u32_max((__u64)l.sec() + r.sec());
	l.nsec_ref() += r.nsec();
	l.normalize();
	return l;
}
inline utime_t& operator+=(utime_t& l, double f) {
	double fs = trunc(f);
	double ns = (f - fs) * 1000000000.0;
	l.sec_ref() = cap_to_u32_max(l.sec() + (__u64)fs);
	l.nsec_ref() += (long)ns;
	l.normalize();
	return l;
}

inline utime_t operator-(const utime_t& l, const utime_t& r) {
	return utime_t( l.sec() - r.sec() - (l.nsec()<r.nsec() ? 1:0),
			l.nsec() - r.nsec() + (l.nsec()<r.nsec() ? 1000000000:0) );
}
inline utime_t& operator-=(utime_t& l, const utime_t& r) {
	l.sec_ref() -= r.sec();
	if (l.nsec() >= r.nsec())
		l.nsec_ref() -= r.nsec();
	else {
		l.nsec_ref() += 1000000000L - r.nsec();
		l.sec_ref()--;
	}
	return l;
}
inline utime_t& operator-=(utime_t& l, double f) {
	double fs = trunc(f);
	double ns = (f - fs) * 1000000000.0;
	l.sec_ref() -= (long)fs;
	long nsl = (long)ns;
	if (nsl) {
		l.sec_ref()--;
		l.nsec_ref() = 1000000000L + l.nsec_ref() - nsl;
	}
	l.normalize();
	return l;
}

inline bool operator>(const utime_t& a, const utime_t& b) {
	return (a.sec() > b.sec()) || (a.sec() == b.sec() && a.nsec() > b.nsec());
}
inline bool operator<=(const utime_t& a, const utime_t& b) {
	return !(operator>(a, b));
}
inline bool operator<(const utime_t& a, const utime_t& b) {
	return (a.sec() < b.sec()) || (a.sec() == b.sec() && a.nsec() < b.nsec());
}
inline bool operator>=(const utime_t& a, const utime_t& b) {
	return !(operator<(a, b));
}

inline bool operator==(const utime_t& a, const utime_t& b) {
	return a.sec() == b.sec() && a.nsec() == b.nsec();
}
inline bool operator!=(const utime_t& a, const utime_t& b) {
	return a.sec() != b.sec() || a.nsec() != b.nsec();
}

// 输出
inline std::ostream& operator<<(std::ostream& out, const utime_t& t) {
	return t.localtime(out);
}

}
