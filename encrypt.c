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
/*  
int main()
{
	PMSG test;
	char key[] = "show me the code";
	memset(&test,0,sizeof(PMSG));
	strcpy(test.data,"niadfadfadfafaf");
	test.len = strlen(test.data);
	md5(test.data,test.len,test.md5);
	printf("len is %d,data is %s\n",test.len,test.data);
	encode(&(test.len),sizeof(int),key);
	encode(test.md5,sizeof(char)*16,key);
	encode(test.data,strlen(test.data),key);
	printf("len is %d,data is %s\n",test.len,test.data);
	decode(&(test.len),sizeof(int),key);
	decode(test.md5,sizeof(char)*16,key);
	decode(test.data,test.len,key);
	printf("len is %d,data is %s\n",test.len,test.data);

	return 0;
}
*/
