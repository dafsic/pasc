#include "pasc.h"

void encode(void *data,int size,char *key)
{
	char *psd = (char *)data;
	char *ped = psd + size;
	char *psk = key;
	char *pek = key + strlen(key);

	while(psd != ped)
	{
		if(psk == pek)
		{
			psk = key;
		}
		*psd = *psd ^ *psk;
		psk++;
		psd++;
	}


}
void decode(void *data,int size,char *key)
{
	encode(data,size,key);
}
