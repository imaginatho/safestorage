#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <stdlib.h>

#include <safe_storage_imp.h>

ISafeStorage *createISafeStorage ( uint32_t flags, uint32_t version )
{
	return new CSafeStorage();
}

