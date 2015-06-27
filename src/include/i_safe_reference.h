#ifndef __I_SAFE_REFERENCE__H__
#define __I_SAFE_REFERENCE__H__

#include <stdint.h>
#include <string>
#include <list>
#include <set>
using namespace std;

class ISafeReference
{
    public:
		virtual ~ISafeReference ( void ) {};
		virtual void onSharedClose ( void ) = 0;
};

#endif
