#include "base64.h"
#include <string.h>
#include <stdio.h>

int main() {
	const char originalValue[] = {0x00, 0x00, 0x00, 0x0a};
	char b64Value[14] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	char decodedValue[9] = {0,0,0,0,0,0,0,0,0};
	int len = 0;
	len = Base64encode(b64Value, originalValue, 4);
	//if (len != 13) {
	//	 return 1;
	//}
        printf("decodedValue = %s\n", b64Value);
	len = Base64decode(decodedValue, b64Value);
	if (len != 4) {
		return 2;
	}
	if (memcmp(originalValue, decodedValue, 4) == 0) {
		return 0;
	}
	return 3;
}

