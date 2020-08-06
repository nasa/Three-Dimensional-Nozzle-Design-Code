#ifndef _NR_H_
#define _NR_H_

#ifndef _FCOMPLEX_DECLARE_T_
typedef struct FCOMPLEX {float r,i;} fcomplex;
#define _FCOMPLEX_DECLARE_T_
#endif /* _FCOMPLEX_DECLARE_T_ */

#ifndef _ARITHCODE_DECLARE_T_
typedef struct {
	unsigned long *ilob,*iupb,*ncumfq,jdif,nc,minint,nch,ncum,nrad;
} arithcode;
#define _ARITHCODE_DECLARE_T_
#endif /* _ARITHCODE_DECLARE_T_ */

#ifndef _HUFFCODE_DECLARE_T_
typedef struct {
	unsigned long *icod,*ncod,*left,*right,nch,nodemax;
} huffcode;
#define _HUFFCODE_DECLARE_T_
#endif /* _HUFFCODE_DECLARE_T_ */

#include <stdio.h>

#if defined(__STDC__) || defined(ANSI) || defined(NRANSI) /* ANSI */

float fmin(float x[]);
void indexx(unsigned long n, float arr[], unsigned long indx[]);
void lnsrch(int n, float xold[], float fold, float g[], float p[], float x[],
	 float *f, float stpmax, int *check, float (*func)(float []));
void lubksb(float **a, int n, int *indx, float b[]);
void ludcmp(float **a, int n, int *indx, float *d);
void newt(float x[], int n, int *check,
	void (*vecfunc)(int, float [], float []));
void polint(float xa[], float ya[], int n, float x, float *y, float *dy);
void sort2(unsigned long n, float arr[], float brr[]);
void spline(float x[], float y[], int n, float yp1, float ypn, float y2[]);
void splint(float xa[], float ya[], float y2a[], int n, float x, float *y);

#else /* ANSI */
/* traditional - K&R */

float fmin();
void indexx();
void lnsrch();
void lubksb();
void ludcmp();
void newt();
void polint();
void sort2();
void spline();
void splint();

#endif /* ANSI */

#endif /* _NR_H_ */
