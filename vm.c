//字节码打印与解释

#include "cc.h"
#include <stdio.h>
#include <malloc.h>

int *data;

void vm_init(void) {
	data = (int*)malloc(MAXSIZE * sizeof(int));
}

static int* print_emit(int *i) {
	#define PRINT_REG(x) do { \
		int y = (x); \
		if(y == IP) printf("IP"); \
		else if(y == BP) printf("BP"); \
		else if(y == SP) printf("SP"); \
		else if(y == AX) printf("AX"); \
		else printf("[%d]", y); \
	} while(0)
	switch(*i) {
	case PUSH: printf("PUSH "); PRINT_REG(*++i); break;
	case POP: printf("POP  "); PRINT_REG(*++i); break;
	case SET: printf("SET  "); PRINT_REG(*++i); printf(" %d", *++i); break;
	case INC: printf("INC  "); PRINT_REG(*++i); printf(" %d", *++i); break;
	case DEC: printf("DEC  "); PRINT_REG(*++i); printf(" %d", *++i); break;
	case JMP: printf("JMP  [%d]", *++i); break;
	case JZ: printf("JZ   [%d]", *++i); break;
	case MOV: printf("MOV  "); PRINT_REG(*++i); printf(" "); PRINT_REG(*++i); break;
	case ADD: printf("ADD"); break;
	case SUB: printf("SUB"); break;
	case MUL: printf("MUL"); break;
	case DIV: printf("DIV"); break;
	case MOD: printf("MOD"); break;
	case ASS: printf("ASS"); break;
	case EQ: printf("EQ"); break;
	case GT: printf("GT"); break;
	case LT: printf("LT"); break;
	case AND: printf("AND"); break;
	case OR: printf("OR"); break;
	case NOT: printf("NOT"); break;
	case AG: printf("AG   %d", *++i); break;
	case AL: printf("AL   %d", *++i); break;
	case VAL: printf("VAL"); break;
	case CALL: printf("CALL %d", *++i); break;
	case CAPI: printf("CAPI %d", *++i); break;
	case EXIT: printf("EXIT"); break;
	default: printf("[%d]", i - emit);
	}
	printf("\n");
	return i;
	#undef PRINT_REG
}

void vm_run(int src, int debug) {
	//print
	if(src) {
		for(int *i = emit; i < e; i++) {
			printf("%d\t", i - emit);
			i = print_emit(i);
		}
	}
	//run..
	data[SP] = data[BP] = AX + 1; //sp = bp = AX + 1;
	data[IP] = 0; //ip = 0;
	while(1) {
		// for(int i = 0; i < data[SP]; i++) {
			// printf("\n%d: %d", i, data[i]);
		// }
		if(debug) {
			printf("\n_%d_%d_%d_%d_\t", data[IP], data[BP], data[SP], data[AX]);
			print_emit(emit + data[IP]);
		}
		int i = emit[data[IP]++];
		int opr1, opr2;
		switch(i) {
		case PUSH:
			opr1 = emit[data[IP]++];
			data[data[SP]++] = data[opr1];
			break;
		case POP:
			opr1 = emit[data[IP]++];
			data[opr1] = data[--data[SP]];
			break;
		case SET:
			opr1 = emit[data[IP]++];
			opr2 = emit[data[IP]++];
			data[opr1] = opr2;
			break;
		case INC:
			opr1 = emit[data[IP]++];
			opr2 = emit[data[IP]++];
			data[opr1] += opr2;
			break;
		case DEC:
			opr1 = emit[data[IP]++];
			opr2 = emit[data[IP]++];
			data[opr1] -= opr2;
			break;
		case JMP:
			opr1 = emit[data[IP]++];
			data[IP] = opr1;
			break;
		case JZ: //jump if zero
			opr1 = emit[data[IP]++];
			if(!data[AX]) data[IP] = opr1;
			break;
		case MOV:
			opr1 = emit[data[IP]++];
			opr2 = emit[data[IP]++];
			data[opr1] = data[opr2];
			break;
		case ADD:
			opr1 = data[AX];
			opr2 = data[--data[SP]];
			data[AX] = opr1 + opr2;
			break;
		case SUB:
			opr1 = data[AX];
			opr2 = data[--data[SP]];
			data[AX] = opr2 - opr1;
			break;
		case MUL:
			opr1 = data[AX];
			opr2 = data[--data[SP]];
			data[AX] = opr1 * opr2;
			break;
		case DIV:
			opr1 = data[AX];
			opr2 = data[--data[SP]];
			data[AX] = opr2 / opr1;
			break;
		case MOD:
			opr1 = data[AX];
			opr2 = data[--data[SP]];
			data[AX] = opr2 % opr1;
			break;
		case ASS:
			opr1 = data[AX];
			opr2 = data[--data[SP]];
			data[opr2] = opr1;
			break;
		case EQ:
			opr1 = data[AX];
			opr2 = data[--data[SP]];
			data[AX] = opr2 == opr1;
			break;
		case GT:
			opr1 = data[AX];
			opr2 = data[--data[SP]];
			data[AX] = opr2 > opr1;
			break;
		case LT:
			opr1 = data[AX];
			opr2 = data[--data[SP]];
			data[AX] = opr2 < opr1;
			break;
		case AND:
			opr1 = data[AX];
			opr2 = data[--data[SP]];
			data[AX] = opr2 && opr1;
			break;
		case OR:
			opr1 = data[AX];
			opr2 = data[--data[SP]];
			data[AX] = opr2 || opr1;
			break;
		case NOT:
			data[AX] = !data[AX];
			break;
		case AG: //address global
			opr1 = emit[data[IP]++];
			data[AX] = opr1;
			break;
		case AL: //address local
			opr1 = emit[data[IP]++];
			data[AX] = data[BP] + opr1;
			break;
		case VAL:
			data[AX] = data[data[AX]];
			break;
		case CALL:
			opr1 = emit[data[IP]++];
			data[data[SP]++] = data[IP]; //push next_ip
			data[IP] = data[data[SP] - opr1 - 2];
			break;
		case CAPI:
			opr1 = emit[data[IP]++];
			api_call(data[data[SP] - opr1 - 1]);
			break;
		case EXIT: return;
		}
	}
}
