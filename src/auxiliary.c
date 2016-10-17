#include "auxiliary.h"

int getBitAmount(int32_t number){
	int i;

	for(i=31; i>=0; i--){
		if( (number>>i)&1 )
			return i+1;
	}

	return 0;
}
