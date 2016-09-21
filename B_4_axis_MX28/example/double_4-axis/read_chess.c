#include <stdio.h>

int main()
{
	int a, b, c;
	FILE *fp = fopen("./chess_game/4.txt", "r");
	while(!feof(fp)) {
		fscanf(fp, "(%d %d)-->%d\n", &a, &b, &c);
		printf("(%d %d)-->%d\n", a, b, c);
	}
	
	fclose(fp);
	fp = NULL;	
}
