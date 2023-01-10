// include
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <complex.h>		//	C++
#include <iomanip>


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

// プロトタイプ宣言
int a_continue();
char *StrToLower(char*);
int input(double*);
double function(double*, double);
std::complex<double> function_complex(double*, std::complex<double>);
std::complex<double> differentiate1(double*, std::complex<double>);
std::complex<double> differentiate2(double*, std::complex<double>, double, int);
std::complex<double> solve_equation_a1(double*, std::complex<double>, double);
bool solve_equation_a2(double*, std::complex<double>*, std::complex<double>*);
int solve_equation(double*, std::complex<double>*, double, double, int);
bool swap(double*, double*, double*, double*);
bool swap_complex(std::complex<double>*, std::complex<double>*, std::complex<double>*, std::complex<double>*);
int input_xmdc(std::complex<double>*, double*, double*, unsigned int*);
int getString(char*, int);

int main() {
	double *heap;
	std::complex<double> x;
	double mesh, delta;
	unsigned int cnt;
	heap = (double *)malloc(sizeof(double) * ALLOC_SIZE);
	element = ALLOC_SIZE;
	if (heap == NULL)  abort();
	while (true) {
		order = input(heap);
		if(order < 1) {
			std::cout << "エラーが発生しました\n";
			abort();
		}
		input_xmdc(&x, &mesh, &delta, &cnt);
		solve_equation(heap, &x, mesh, delta, cnt);
		std::cout.setf(std::ios::fixed, std::ios::floatfield);
		std::cout << "解 : x = " << real(x) << " ± " << fabs(imag(x)) << "i" << std::endl;
		if (a_continue() != 1) {
			break;
		}
	}
	free(heap);
	//heap = NULL;
	return 0;
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

int a_continue() {
	char ask[ASK_STR_SIZE], ask2[ASK_STR_SIZE];
	std::cout << "次の方程式を計算しますか？(Y(Yes) or N(No)) : ";
	if (getString(ask, sizeof(ask)) == FAILURE) abort();
	strcpy(ask2,StrToLower(ask));
	if (strcmp(ask2,"n") == 0 || strcmp(ask2,"no") == 0) {
		return 0;
	} else if (strcmp(ask2,"y") == 0 || strcmp(ask2,"yes") == 0) {
		return 1;
	}
	std::cout << std::endl;
	return -1;
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
	std::cout << "n次関数の係数を次数の高い順に入力してください。" << std::endl;
	for( ; ; i++) {
		if (i % ALLOC_SIZE == 0 && i != 0) {
			num = (double *)realloc(num, sizeof(double) * ALLOC_SIZE * (i + 1));
			element = ALLOC_SIZE * (i + 1);
		}
		if (getString(indata, sizeof(indata)) == FAILURE) abort();
		if (strcmp(StrToLower(indata), "") == 0 || strcmp(StrToLower(indata), "end") == 0)
			break;
		num[i] = atof(indata);
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
	int i,j;
	double y;
	for (i = order, j = 0; i > -1; i--, j++) {
		if (i > 0) {
			y += coefficient[j] * pow(x,i);
		} else if (i == 0) {
			y += coefficient[j];
		}
	}
	return y;
}

std::complex<double> function_complex(double *coefficient, std::complex<double> x)
{
	int i, j;
	std::complex<double> y;
	for (i = order, j = 0; i > -1; i--, j++) {
		if (i > 0) {
			y += coefficient[j] * pow(x, i);
		} else if (i == 0) {
			y += coefficient[j];
		}
	}
	return y;
}

std::complex<double> differentiate1(double *coefficient, std::complex<double> x)
{
	std::complex<double> yd = 0.0;
	int i, j;
	for (i = order, j = 0; i > 0; i--, j++) {
		if (i > 0) {
			if (abs(x) == 0 && i - 1 == 0) {
				yd += 0;
			} else {
				yd += coefficient[j] * i * pow(x, (i - 1));
			}
		} else if (i == 0) {
			yd += coefficient[j];
		}
	}
	return yd;
}

std::complex<double> differentiate2(double *coefficient, std::complex<double> x, double mesh, int way) {
	std::complex<double> yd = 0.0;
	std::complex<double> max, min;
	switch (way) {
		case 1:
			min = x;
			max = x + (1.0 / mesh);
			yd = (function_complex(coefficient, max) - function_complex(coefficient, min)) / (1.0 / mesh);
			break;
		case 2:
			min = x - (1.0 / mesh);
			max = x;
			yd = (function_complex(coefficient, max) - function_complex(coefficient, min)) / (1.0 / mesh);
			break;
		default:
			min = x - (1.0 / (mesh * 2.0));
			max = x + (1.0 / (mesh * 2.0));
			yd = (function_complex(coefficient, max) - function_complex(coefficient, min)) / (1.0 / mesh);
			break;
	}
	return yd;
}

std::complex<double> solve_equation_a1(double *coefficient, std::complex<double> x, double mesh) {
	std::complex<double> a, b, c;
	int i,j;
	if (mesh == 0.0) {
		a = differentiate1(coefficient, x);
	} else {
		a = differentiate2(coefficient, x, mesh, 0);
	}
	c = function_complex(coefficient, x);
	b = c - (a * x);
	if (a != 0.0) {
		return (-b / a);
	} else {
		return x;
	}
}

bool solve_equation_a2(double *coefficient, std::complex<double> *x1, std::complex<double> *x2) {
	std::complex<double> y1, y2, a, b, x;
	y1 = function_complex(coefficient, *x1);
	y2 = function_complex(coefficient, *x2);
	swap_complex(x1, x2, &y1, &y2);			//	y1 < y2  ( f(x1) < f(x2) )
	if (abs(y1) > 0 || abs(y2) < 0) {
		return false;
	} else if (abs(y1) == 0) {
		*x2 = *x1;
		return true;
	} else if (abs(y2) == 0) {
		*x1 = *x2;
		return true;
	}
	a = (y2 - y1) / (*x2 - *x1);
	b = y1 - a * *x1;
	x = -b / a;
	if (abs(function_complex(coefficient,x)) < 0) {
		*x1 = x;
	} else if (abs(function_complex(coefficient,x)) > 0) {
		*x2 = x;
	} else {
		return false;
	}
	return true;
}

int solve_equation(double *coefficient, std::complex<double> *x, double mesh, double delta, int cnt)
{
	double xp = 0.0;
	std::complex<double> y, yd, min, max;
	int i, a = 0;
	std::cout << "\tf(x)\t\tf'(x)\t\tx" << std::endl;
	y = function_complex(coefficient, *x);
	if (mesh == 0) {
		yd = differentiate1(coefficient, *x);
	} else {
		yd = differentiate2(coefficient, *x, mesh, 0);
	}
	std::cout << "0回目\t" << y << "\t\t" << yd << "\t\t" << *x << std::endl;
	for (i = 0 ; i < cnt; i++) {
		if (a == 0) {
			 *x = solve_equation_a1(coefficient, *x+xp, mesh);
		} else if (a == 1) {
			 solve_equation_a2(coefficient, &min, &max);
		}
		if (i == 0) {
			min = *x;
			max = *x;
		} else if (i == 1) {
			if (abs(min) > abs(*x)) {
				min = *x;
			} else if (abs(max) < abs(*x)) {
				max = *x;
			}
		} else {
			if (abs(min) > abs(*x)) {
				if (abs(function_complex(coefficient, *x)) < abs(y)) {
					a = 0;
				} else if(abs(function_complex(coefficient, *x)) > abs(y)) {
					a = 1;
				}
				min = *x;
			} else if(abs(max) < abs(*x)) {
				if(abs(function_complex(coefficient,*x)) < abs(y)) {
					a = 0;
				} else if(abs(function_complex(coefficient,*x)) > abs(y)) {
					a = 1;
				}
				max = *x;
			} else {
				a = 0;
			}
		}
		y = function_complex(coefficient, *x);
		if (delta == 0) {
			if (abs(y) == 0) {
				std::cout << i + 1 << "回目\t" << y << "\t\t" << yd << "\t\t" << *x << std::endl;
				break;
			}
		} else {
			if (abs(y) <= (1.0 / delta)) {
				std::cout << i + 1 << "回目\t" << y << "\t\t" << yd << "\t\t" << *x << std::endl;
				break;
			}
		}
		if (mesh == 0) {
			yd = differentiate1(coefficient, *x);
		} else {
			yd = differentiate2(coefficient, *x, mesh, 0);
		}
		std::cout << i + 1 << "回目\t" << y << "\t\t" << yd << "\t\t" << *x << std::endl;
		if (delta == 0) {
			if (abs(yd) <= EPS & abs(y) > EPS) {
				xp += EPS;
			} else {
				xp = 0.0;
			}
		} else {
			if (abs(yd) <= (1.0 / delta) & abs(y) > (1.0 / delta)) {
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

bool swap_complex(std::complex<double> *a, std::complex<double> *b, std::complex<double> *c, std::complex<double> *d) {
	std::complex<double> i;
	if (abs(*c) > abs(*d)) {
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

int input_xmdc(std::complex<double> *x, double *mesh, double *delta, unsigned int *cnt) {
	char indata[STR_SIZE];
	std::cout << "初期値(1 + 2i ならば (1,2) と入力) : ";
	std::cin >> *x;
	
	printf("微分の精度（1以上 or 0）: ");
	do {
		if (getString(indata, sizeof(indata)) == FAILURE) abort();
		*mesh = fabs(strtod(indata, NULL));
	} while (*mesh < MIN_ACCURACY && *mesh != 0);
	
	printf("誤差範囲（1以上 or 0） : ");
	do {
		if (getString(indata, sizeof(indata)) == FAILURE) abort();
		*delta = fabs(strtod(indata, NULL));
	} while (*delta < MIN_ERROR && *delta != 0);
	
	printf("ループ回数（1以上）: ");
	do {
		if (getString(indata, sizeof(indata)) == FAILURE) abort();
		*cnt = abs((int)strtol(indata, NULL, 0));
	} while (*cnt < MIN_COUNT);
	return SUCCESS;
}
