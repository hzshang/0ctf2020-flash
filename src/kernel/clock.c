#include "stdint.h"
#include "mipsregs.h"

#define TIMER0_INTERVAL  0x800000
// void reload_timer()
// {
//     uint32_t counter = read_c0_count();
//     counter += TIMER0_INTERVAL;
//     write_c0_compare(counter);
// }

// void timer_init(void)
// {
//   reload_timer();
// }
