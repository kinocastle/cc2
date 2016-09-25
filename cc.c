//main

#include "cc.h"
#include <stdio.h>
#include <malloc.h>
#include <string.h>

static void printint(void) {
	int i = api_getint(1);
	printf("%d", i);
}

static void printchar(void) {
	char c = api_getchar(1);
	printf("%c", c);
}

static void printstr(void) {
	char *s = api_getstr(1);
	printf("%s", s);
}

static void scanint(void) {
	int i;
	scanf("%d", &i);
	api_setint(i);
}

static void scanchar(void) {
	char c;
	scanf("%c", &c);
	api_setchar(c);
}

static void scanstr(void) {
	char s[BUFSIZE];
	scanf("%s", s);
	api_setstr(s);
}

static void endl(void) {
	printf("\n");
}

static void space(void) {
	printf(" ");
}

int main(int argc, char *argv[]) {
	int src = 0, debug = 0;
	char *fname = NULL;
	FILE *fp;
	if(argc < 2) { printf("error2!\n"); exit(-1); }
	for(int i = 1; i < argc; i++) {
		if(!strcmp(argv[i], "-s")) {
			src = 1;
		} else if(!strcmp(argv[i], "-d")) {
			debug = 1;
		} else {
			char *j;
			if((j = strrchr(argv[i], '.'))) {
				if(!strcmp(j, ".c")) fname = argv[i];
				else { printf("error3!\n"); exit(-1); }
			} else { printf("error4!\n"); exit(-1); }
		}
	}
	if(!fname) { printf("error5!\n"); exit(-1); }
	if(!(fp = fopen(fname, "r"))) { printf("error6!\n"); exit(-1); }
	
	declare_init();
	token_init();
	id_init();
	expr_init();
	vm_init();
	api_init();
	
	{ int i = fread(p, sizeof(char), MAXSIZE, fp); p[i] = '\0'; }//printf("%s",p);
	fclose(fp);
	
	//reg api
	api_register(printint, "int printint(int i);");
	api_register(printchar, "int printchar(char c);");
	api_register(printstr, "int printstr(char *s);");
	api_register(scanint, "int scanint();");
	api_register(scanchar, "char scanchar();");
	api_register(scanstr, "char* scanstr();");
	api_register(endl, "int endl();");
	api_register(space, "int space();");
	
	//into code
	*e++ = AG; int *_main = e++;
	*e++ = PUSH; *e++ = AX;
	*e++ = SET; *e++ = AX; int *_exit = e++;
	*e++ = PUSH; *e++ = AX;
	*e++ = CALL; *e++ = 0;//*e++ = JMP; int *_main = e++;
	*_exit = e - emit; *e++ = EXIT;
	
	//parse
	next();
	while(strcmp(tks, "") || tki != -1) {
		declare(GLO);
		next();
	}
	*_main = getid("main") -> offset;
	
	//run
	run_vm(src, debug);
	return 0;
}
