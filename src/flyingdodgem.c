#include "flyingdodgem_types.h"

void dummy_entry(void) {}

#if defined(_MSC_VER) && !defined(FLYINGDODGEM_SDL)
int main(void)
{
    dummy_entry();
    return 0;
}
#endif
