#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main()
{
	float cpu = 0.2;
	float mem = 200;
	float rx = 20;
	float tx = 30;
	printf("%.2f,%.0f,%.2f,%.2f", cpu, mem, rx, tx);
	//printf("%.1f,%.0f,%.0f,%.0f", cpu, mem, rx, tx);
   return 1;
}
