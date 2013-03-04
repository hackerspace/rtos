#include <c_entry.h>
#include <string.h>

typedef void (*fp)(void);
extern fp _reset;

void c_entry(void) {
  fp tbl[20];

  tbl[0] = (fp)0x00004000;
  tbl[1] = _reset;

  memcpy(0x0, tbl, sizeof(fp)*20);

  for (;;) {
  }
}
