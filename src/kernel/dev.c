#include "dev.h"
#include "stdint.h"


uint32_t read_cop0_status(void)
{
  uint32_t val;
  asm volatile("mfc0 %0, $12" : "=r" (val));
  return val;
}

void write_cop0_status(uint32_t val)
{
  asm volatile("mtc0 %0, $12" : : "r" (val));
}
uint8_t inb(uint16_t port)
{
  return *((uint8_t *) (IO_PORT_BASE + port));
}
void outb(uint16_t port, uint8_t data)
{
  *((uint8_t *) (IO_PORT_BASE + port)) = data;
}
