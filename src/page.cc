#include <unistd.h>
#include "namespace.h"
#include "page.h"

namespace TOPNSPC {

// page size crap, see page.h
int _get_bits_of(int v)
{
	int n = 0;
	while (v) {
		n++;
		v = v >> 1;
	}
	return n;
}

unsigned _page_size = sysconf(_SC_PAGESIZE);
unsigned _page_shift = _get_bits_of(_page_size - 1);
unsigned long _page_mask = ~(unsigned long)(_page_size - 1);

}
