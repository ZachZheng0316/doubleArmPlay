#include <stdio.h>

int main()
{
	FILE *fp = fopen("./chess_game/1.txt", "w+");
	
	//向文件流中写入数据
	fprintf(fp, "(-3 3)-->18\n");
	fprintf(fp, "(-3 5)-->10\n");
	fprintf(fp, "(-1 1)-->28\n");
	fprintf(fp, "(-1 7)-->21\n");
	fprintf(fp, "(1 1)-->5\n");
	fprintf(fp, "(1 7)-->2\n");
	fprintf(fp, "(3 3)-->17\n");
	fprintf(fp, "(3 5)-->8\n");
	
	fclose(fp);
	fp = NULL;
}
