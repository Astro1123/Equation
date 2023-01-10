// include
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
//#include <complex.h>		//	C++

// 定数宣言
#define ALLOC_SIZE 100
#define STR_SIZE 256
#define ASK_STR_SIZE 5

#define EPS 1.0 / 1000000.0

#define SUCCESS 0
#define FAILURE -1

#define MIN_ACCURACY 1
#define MIN_ERROR 1
#define MIN_COUNT 1

// 変数宣言
long int order;
unsigned long int element;

//	プロトタイプ宣言
bool a_continue(void);
char *StrToLower(char*);
int input(double*);
double function(double*, double);
double differentiate1(double*, double);
double differentiate2(double*, double, double, int);
double solve_equation_a1(double*, double, double);
bool solve_equation_a2(double*, double*, double*);
int solve_equation(double*, double*, double, double, int);
bool swap(double*, double*, double*, double*);
int input_xmdc(double*, double*, double*, unsigned int*);
int getString(char*, int);

int main(void) {
	double *heap, x, error, diff;
	unsigned int cnt;
	
	heap = (double *)malloc(sizeof(double) * ALLOC_SIZE);
	element = ALLOC_SIZE;
	if (heap == NULL) abort();
	while (true) {
		order = input(heap);
		if (order < 1) {
			printf("エラーが発生しました\n");
			abort();
		}
		input_xmdc(&x, &diff, &error, &cnt);
		solve_equation(heap, &x, diff, error, cnt);
		printf("解 : x = %f\n",x);
		if (!a_continue()) {
			break;
		}
	}
	free(heap);
	//heap = NULL;
	return SUCCESS;
}

int getString(char *str, int size) {
	int len;
	
    if (fgets(str, size, stdin) != NULL) {
    	len = strlen(str);
        if (str[len - 1] == '\n') {
            str[--len] = '\0';
        } else {
            while (getchar() != '\n') {}
        }
        return len;
    }
    return FAILURE;
}

bool a_continue(void) {
	char ask[ASK_STR_SIZE], ask2[ASK_STR_SIZE];
	
	while (true) {
		printf("次の方程式を計算しますか？(Y(Yes) or N(No)) : ");
		if (getString(ask, sizeof(ask)) == FAILURE) abort();
		strcpy(ask2, StrToLower(ask));
		if (strcmp(ask2, "n") == 0 || strcmp(ask2, "no") == 0) {
			return false;
		} else if (strcmp(ask2, "y") == 0 || strcmp(ask2, "yes") == 0) {
			return true;
		}
		puts("");
	}
}

char *StrToLower(char *s) {
	char *p;
	
	for (p = s; *p; p++)
		*p = tolower(*p);
	return (s);
}

int input(double *num) {
	char indata[STR_SIZE];
	int i = 0;
	
	puts("n次関数の係数を次数の高い順に入力してください。");
	for(;;i++) {
		if (i % ALLOC_SIZE == 0 & i != 0) {
			num = (double *)realloc(num,sizeof(double) * ALLOC_SIZE * (i + 1));
			element = ALLOC_SIZE * (i + 1);
		}
		if (getString(indata, sizeof(indata)) == FAILURE) abort();
		if (strcmp(StrToLower(indata),"") == 0 || strcmp(StrToLower(indata),"end") == 0)
			break;
		num[i] = strtod(indata, NULL);
	}
	
	/*
	for(i = 0;i < malloc_usable_size(num) / sizeof(num[0]);i++)
		printf("%f\n",num[i]);
	puts("");
	*/
	i--;
	return i;
}

double function(double *coefficient, double x) {
	int i, j;
	double y = 0;
	
	for (i = order, j = 0 ; i > -1; i--, j++) {
		if (i > 0) {
			y += coefficient[j] * pow(x,i);
		} else if (i == 0) {
			y += coefficient[j];
		}
	}
	return y;
}

double differentiate1(double *coefficient, double x) {
	double yd = 0.0;
	int i, j;
	
	for (i = order, j = 0; i > 0; i--, j++) {
		if (i > 0) {
			if( x == 0 && i - 1 == 0 ) {
				yd += 0;
			} else {
				yd += coefficient[j] * i * pow(x,(i - 1));
			}
		} else if (i == 0) {
			yd += coefficient[j];
		}
	}
	return yd;
}

double differentiate2(double *coefficient, double x, double diff, int way) {
	double yd, max, min;
	
	switch (way) {
		case 1:
			min = x;
			max = x + (1.0 / diff);
			yd = (function(coefficient,max) - function(coefficient,min)) / (1.0 / diff);
			break;
		case 2:
			min = x - (1.0 / diff);
			max = x;
			yd = (function(coefficient,max) - function(coefficient,min)) / (1.0 / diff);
			break;
		default:
			min = x - (1.0 / (2.0 * diff));
			max = x + (1.0 / (2.0 * diff));
			yd = (function(coefficient,max) - function(coefficient,min)) / (1.0 / diff);
			break;
	}
	return yd;
}

/* ニュートン法 */
double solve_equation_a1(double *coefficient, double x, double diff) {
	double a, b, c;
	int i, j;
	if (diff == 0.0) {
		a = differentiate1(coefficient, x);
	} else {
		a = differentiate2(coefficient, x, diff, 0);
	}
	c = function(coefficient, x);
	b = c - (a * x);
	if (a != 0.0) {
		return (-b / a);
	} else {
		return x;
	}
}

/* 挟み撃ち法 */
bool solve_equation_a2(double *coefficient, double *x1, double *x2) {
	double y1, y2, a, b, x;
	
	y1 = function(coefficient, *x1);
	y2 = function(coefficient, *x2);
	/*	if y1 < y2  ( f(x1) < f(x2) ) */
	/* then swap x1 and x2 */
	swap(x1, x2, &y1, &y2);
	if (y1 > 0 || y2 < 0) {
		return false;
	} else if (y1 == 0) {
		*x2 = *x1;
		return true;
	} else if (y2 == 0) {
		*x1 = *x2;
		return true;
	}
	a = (y2 - y1) / (*x2 - *x1);
	b = y1 - a * *x1;
	x = -b / a;
	if (function(coefficient, x) < 0) {
		*x1 = x;
	} else if (function(coefficient, x) > 0) {
		*x2 = x;
	} else {
		return false;
	}
	return true;
}

int solve_equation(double *coefficient, double *x, double diff, double delta, int cnt) {
	double min, max, y, yd, xp = 0.0;
	int i, a = 0;
	
	puts("\tf(x)\t\tf'(x)\t\tx");
	y = function(coefficient, *x);
	if (diff == 0) {
		yd = differentiate1(coefficient, *x);
	} else {
		yd = differentiate2(coefficient, *x, diff, 0);
	}
	printf("0回目\t%f\t%f\t%f\n",y,yd,*x);
	for (i = 0; i < cnt; i++) {
		if (a == 0) {
			 *x = solve_equation_a1(coefficient, *x + xp, diff);
		} else if(a == 1) {
			 solve_equation_a2(coefficient, &min, &max);
		}
		if (i == 0) {
			min = *x;
			max = *x;
		} else if (i == 1) {
			if (min > *x) {
				min = *x;
			} else if (max < *x) {
				max = *x;
			}
		} else {
			if (min > *x) {
				if (fabs(function(coefficient, *x)) < y) {
					a = 0;
				} else if (fabs(function(coefficient, *x)) > y) {
					a = 1;
				}
				min = *x;
			} else if (max < *x) {
				if (fabs(function(coefficient, *x)) < y) {
					a = 0;
				} else if (fabs(function(coefficient, *x)) > y) {
					a = 1;
				}
				max = *x;
			} else {
				a = 0;
			}
		}
		y = function(coefficient, *x);
		if (delta == 0) {
			if (fabs(y) == 0) {
				printf("%d回目\t%f\t%f\t%f\n",(i + 1), y, yd, *x);
				break;
			}
		} else {
			if (fabs(y) <= (1.0 / delta)) {
				printf("%d回目\t%f\t%f\t%f\n", (i + 1), y, yd, *x);
				break;
			}
		}
		if (diff == 0) {
			yd = differentiate1(coefficient,*x);
		} else {
			yd = differentiate2(coefficient,*x,diff,0);
		}
		printf("%d回目\t%f\t%f\t%f\n",(i + 1), y, yd, *x);
		if (delta == 0) {
			if (fabs(yd) <= EPS) {
				xp += EPS;
			} else {
				xp = 0.0;
			}
		} else {
			if (fabs(yd) <= (1.0 / delta) && fabs(y) > (1.0 / delta)) {
				xp += EPS;
			} else {
				xp = 0.0;
			}
		}
	}
	return SUCCESS;
}

bool swap(double *a, double *b, double *c, double *d) {
	double i;
	if (*c > *d) {
		i = *a;
		*a = *b;
		*b = i;
		
		i = *c;
		*c = *d;
		*d = i;
		return true;
	}
	return false;
}

int input_xmdc(double *x, double *accuracy, double *error, unsigned int *cnt)
{
	char indata[STR_SIZE];
	
	printf("初期値 : ");
	if (getString(indata, sizeof(indata)) == FAILURE) abort();
	*x = strtod(indata, NULL);
	
	printf("微分の精度（1以上 or 0）: ");
	do {
		if (getString(indata, sizeof(indata)) == FAILURE) abort();
		*accuracy = fabs(strtod(indata, NULL));
	} while (*accuracy < MIN_ACCURACY && *accuracy != 0);
	
	printf("誤差範囲（1以上 or 0） : ");
	do {
		if (getString(indata, sizeof(indata)) == FAILURE) abort();
		*error = fabs(strtod(indata, NULL));
	} while (*error < MIN_ERROR && *error != 0);
	
	printf("ループ回数（1以上）: ");
	do {
		if (getString(indata, sizeof(indata)) == FAILURE) abort();
		*cnt = abs((int)strtol(indata, NULL, 0));
	} while (*cnt < MIN_COUNT);
	return SUCCESS;
}
