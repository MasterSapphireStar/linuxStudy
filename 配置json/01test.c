#include <stdio.h>

int add(int a, int b)
{
	return a + b;
}

int main()
{
	int num;
	num = add(1, 99);
	printf("%d\n", num);
	return 0;
}
