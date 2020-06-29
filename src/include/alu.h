#ifndef ALU_H
#define ALU_H

static inline unsigned int __mulu10(unsigned int n)
{
  return (n<<3)+(n<<1);
}

static inline unsigned int __divu10(unsigned int n) {
  unsigned int q, r;
  q = (n >> 1) + (n >> 2);
  q = q + (q >> 4);
  q = q + (q >> 8);
  q = q + (q >> 16);
  q = q >> 3;
  r = n - __mulu10(q);
  return q + ((r + 6) >> 4);
}

static inline unsigned __divu5(unsigned int n) {
  unsigned int q, r;
  q = (n >> 3) + (n >> 4);
  q = q + (q >> 4);
  q = q + (q >> 8);
  q = q + (q >> 16);
  r = n - q*5;
  return q + (13*r >> 6);
}



#endif