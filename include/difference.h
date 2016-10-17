#ifndef _DIFFERENCE_H_
#define _DIFFERENCE_H_

int32_t* differenceEncode(int32_t *orig, int orig_size, int *dest_size);

int32_t* differenceDecode(int32_t *orig, int orig_size, int *dest_size);

#endif
