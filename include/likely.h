#pragma once

/*
 * Likely / Unlikely macros
 */
#ifndef likely
#define likely(x)       __builtin_expect((x),1)
#endif
#ifndef unlikely
#define unlikely(x)     __builtin_expect((x),0)
#endif
#ifndef expect
#define expect(x, hint) __builtin_expect((x),(hint))
#endif
