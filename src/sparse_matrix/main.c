/**
 * @file   main.cpp
 * @author WANG Heyu <wangheyu@zju.edu.cn>
 * @date   Mon Dec 26 03:43:56 2016
 * 
 * @brief �ñ�׼Cʵ��2D Poisson�������, Ӧ����CSRϡ������ʽ, ���ṩ
 * ��GS, SOR��CG���Խ���. �����ڽ�ѧ��ʾ, ���ܱ�֤ʵ�ʼ���Ч��. ֻ����
 * �ڴ�D�������������μ�������.
 * 
 */

/// ���������
#define _SOLVER_PCG_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "sparse_crs.h"

double X0;			/**< �����������½� x ����. */
double Y0;			/**< �����������½� y ����. */
double X1;			/**< �����������Ͻ� x ����. */
double Y1;			/**< �����������Ͻ� y ����. */
int N;				/**< ��������ֶ���. */
double h;			/**< ��������ֶγ���. */
int n;				/**< �����ɶ�. */
double u_old;			/**< ����ֵ�⻺��, ����SOR����. */
double omega;			/**< SOR���ϵ��. */
double tol; 	                /**< ���Խ����������. */
int maxit;			/**< ���Խ�������������. */
int mode;			/**< �����ѡ��, 0 = GS, 1 = SOR, 2 = CG, 3 = PCG. */

/** 
 * D�ϱ߽�����.
 * 
 * @param x �߽�����.
 * @param y �߽�����.
 * 
 * @return �߽纯��ֵ.
 */
double bnd(double x, double y);

/** 
 * Դ��.
 * 
 * @param x Դ������.
 * @param y Դ������.
 * 
 * @return Դ��ֵ.
 */
double source(double x, double y);

/// ������.
int main (int argc, char *argv[])
{
    clock_t t;		/**< ʱ��ͳ�ƻ��� */
    /// ��������
    FILE *input;		/**< ���������ļ��� */
    FILE *output;		/**< �������ļ��� */
    char c;			/**< �ַ����뻺�� */
    double rTr;		/**< CG, r��������*/
    double rTr_old;		/**< CG, r������һ������ */
    double pTAp;		/**< CG, ������С */
    double alpha;		/**< CG, ����� */
    double beta;		/**< CG, �����У�� */
    /// ׼���ڴ����
    double *u;		/**< ��ֵ�� */
    double *r;		/**< ���� */
    double *y;		/**< CG Ԥ������� */
    double *p;		/**< ����� */
    double *Ap;		/**< ������������ */
    double *b;		/**< �Ҷ��� */

    struct PATTERN pA;	/**< ϵ������ģ�� */
    struct MATRIX A;	/**< ϵ������ */

    int i;			/**< �б��. (x��) */
    int j;			/**< �б��. (y��) */
    double err;     	/**< ����������, 2-��. */
    int k;			/**< �������������� */

    int *row;		/**< ϡ������������ӿ� */
    int *col;		/**< ϡ������������ӿ� */
    double *ele;		/**< ϡ��������Ԫ�ӿ� */
    double error;

    input = fopen("config","r");
    output = fopen("re.m","w");
    fscanf(input,"%lf", &X0);
    while ((c=fgetc(input)) != '\n');
    fscanf(input,"%lf", &Y0);
    while ((c=fgetc(input)) != '\n');
    fscanf(input,"%lf", &X1);
    while ((c=fgetc(input)) != '\n');
    fscanf(input,"%lf", &Y1);
    while ((c=fgetc(input)) != '\n');
    fscanf(input,"%d", &N);
    while ((c=fgetc(input)) != '\n');
    fscanf(input,"%lf", &omega);
    while ((c=fgetc(input)) != '\n');
    fscanf(input,"%lf", &tol);
    while ((c=fgetc(input)) != '\n');
    fscanf(input,"%d", &maxit);
    while ((c=fgetc(input)) != '\n');
    fscanf(input,"%d", &mode);
    h = ((X1 - X0) / (N + 1));
    n = (N * N);
    /// ����У��
    if (X1 < X0 || Y1 < Y0)
    {
	printf ("Region error!\n");
	exit(-1);
    }
    if (N < 0)
    {
	printf ("Nodes error!\n");
	exit(-1);
    }
    /// �û�У��
    printf("Region: [%lf, %lf] x [%lf, %lf];\n", X0, X1, Y0, Y1);
    printf("No. of Nodes: %d x %d;\n", N, N);
    printf("h = %lf, n = %d\n", h, n);
    printf("tol = %g, maxit = %d\n", tol, maxit);
    printf("mode = %d, 0 = GS, 1 = SOR, 2 = CG, 3 = PCG-SSOR\n", mode); 
    printf("Press enter to continue...\n");
    getchar(); 

    /// �����ڴ�ռ�
    if ((u = (double *)malloc(n * sizeof(double))) == NULL ||
	(r = (double *)malloc(n * sizeof(double))) == NULL ||
	(p = (double *)malloc(n * sizeof(double))) == NULL ||
	(b = (double *)malloc(n * sizeof(double))) == NULL ||
	(y = (double *)malloc(n * sizeof(double))) == NULL ||
	(Ap = (double *)malloc(n * sizeof(double))) == NULL)
    {
	printf ("Out of memory!\n");
	exit(-1);
    }

    t = clock();
    /// ׼��ϵ������ģ��.
    init_pattern(&pA, n, n, 5);
    /// Ui,j
    for (i = 0; i < N; i++)
	for (j = 0; j < N; j++)
	    set_pattern(&pA, j * N + i, j * N + i);
    /// Ui+1,j
    for (j = 0; j < N; j++)
	for (i = 0; i < N - 1; i++)
	    set_pattern(&pA, j * N + i, j * N + i + 1);
    /// Ui-1,j
    for (j = 0; j < N; j++)
	for (i = 1; i < N; i++)
	    set_pattern(&pA, j * N + i, j * N + i - 1);
    /// Ui1,j-1
    for (i = 0; i < N; i++)
	for (j = 1; j < N; j++)
	    set_pattern(&pA, j * N + i, j * N + i - N);
    /// Ui1,j+1
    for (i = 0; i < N; i++)
	for (j = 0; j < N - 1; j++)
	    set_pattern(&pA, j * N + i, j * N + i + N);
    compress_pattern(&pA);
    init_matrix(&A, &pA);
    /// Ui,j, �Ҷ���
    for (i = 0; i < N; i++)
	for (j = 0; j < N; j++)
	{
	    add_matrix(&A, j * N + i, j * N + i, 4.0);
	    b[j * N + i] = h * h * source(X0 + (i + 1) * h, Y0 + (j + 1) * h);
	}
    /// Ui+1,j, �ұ߽�
    for (j = 0; j < N; j++)
    {
	for (i = 0; i < N - 1; i++)
	    add_matrix(&A, j * N + i, j * N + i + 1, -1.0);
	b[j * N + N - 1] += bnd(X1, Y0 + (j + 1) * h);
    }
    /// Ui-1,j, ��߽�
    for (j = 0; j < N; j++)
    {
	for (i = 1; i < N; i++)
	    add_matrix(&A, j * N + i, j * N + i - 1, -1.0);
	b[j * N] += bnd(X0, Y0 + (j + 1) * h);
    }
    /// Ui1,j-1, �±߽�
    for (i = 0; i < N; i++)
    {
	for (j = 1; j < N; j++)
	    add_matrix(&A, j * N + i, j * N + i - N, -1.0);
	b[i] += bnd(X0 + (i + 1) * h, Y0);
    }
    /// Ui1,j+1, �ϱ߽�
    for (i = 0; i < N; i++)
    {
	for (j = 0; j < N - 1; j++)
	    add_matrix(&A, j * N + i, j * N + i + N, -1.0);
	b[(N - 1) * N + i] += bnd(X0 + (i + 1) * h, Y1);

    }
    t = clock() - t;
    printf ("The assembling tooks me %d clicks (%f seconds).\n",(int)t,((float)t)/CLOCKS_PER_SEC);
    for (i = 0; i < n; i ++)
	printf("b[%d] = %f\n", i, b[i]);
    /// ���������.
    t = clock();
    if (mode == 2)
    {
	for (i = 0; i < n; i++)
	    u[i] = 1.0;
	/// ��ȡϡ��������ݽӿ�����߲���Ч��.
	row = pA.row;
	col = pA.col;
	ele = A.ele;
	rTr = 0.0;
	for (i = 0; i < n; i++)
	{
	    r[i] = -b[i];
	    for (j = row[i]; j < row[i + 1]; j++)
		r[i] += ele[j] * u[col[j]];
	    p[i] = -r[i];
	    rTr += r[i] * r[i];
	}
	k = 0;
	while (sqrt(rTr) / n > tol)
	{
	    pTAp = 0;
	    for (i = 0; i < n; i++)
	    {
		Ap[i] = 0;
		for (j = row[i]; j < row[i + 1]; j++)
		    Ap[i] += ele[j] * p[col[j]];
		pTAp += p[i] * Ap[i];
	    }
	    alpha = rTr / pTAp;
	    rTr_old = rTr;
	    rTr = 0;
	    for (i = 0; i < n; i++)
	    {
		u[i] += alpha * p[i];
		r[i] += alpha * Ap[i];
		rTr += r[i] * r[i];
	    }
	    beta = rTr / rTr_old;
	    for (i = 0; i < n; i++)
		p[i] = -r[i] + beta * p[i];
	    k++;
	}
    }
    else if (mode == 3)
    {
	for (i = 0; i < n; i++)
	{
	    y[i] = u[i] = 1.0;
	}
	/// ��ȡϡ��������ݽӿ�����߲���Ч��.
	row = pA.row;
	col = pA.col;
	ele = A.ele;
	rTr = 0.0;
	res(&A, u, b, r);
//	pc_id(&A, r, y);
	pc_ssor(&A, r, y, omega);
	for (i = 0; i < n; i++)
	{
	    p[i] = -r[i];
	    rTr += r[i] * y[i];
	}
	k = 0;
	err = rTr;
	while (sqrt(err) / n > tol)
	{
	    pTAp = 0;
	    for (i = 0; i < n; i++)
	    {
		Ap[i] = 0;
		for (j = row[i]; j < row[i + 1]; j++)
		    Ap[i] += ele[j] * p[col[j]];
		pTAp += p[i] * Ap[i];
	    }
	    alpha = rTr / pTAp;
	    rTr_old = rTr;
	    rTr = 0;
	    err = 0;
	    for (i = 0; i < n; i++)
	    {
		u[i] += alpha * p[i];
		r[i] += alpha * Ap[i];
		/// ������ʵ����.
		err += r[i] * r[i];
	    }
//		pc_id(&A, r, y);
	    pc_ssor(&A, r, y, omega);
	    for (i = 0; i < n; i++)
		rTr += r[i] * y[i];
	    beta = rTr / rTr_old;
	    for (i = 0; i < n; i++)
		p[i] = -y[i] + beta * p[i];
	    k++;
	}
    }
    else if (mode == 0)
    {
	for (i = 0; i < n; i++)
	    u[i] = 1.0;
	err = 1.0;
	k = 0;
	while (err > tol)
	{
	    gs_step(&A, u, b);
	    err = res_norm(&A, u, b) / n;
	    k++;
	    if (k > maxit)
	    {
		printf ("not convergent!\n");
		break;
	    }
	}
    }
    else if (mode == 1)
    {
	for (i = 0; i < n; i++)
	    u[i] = 1.0;
	err = 1.0;
	k = 0;
	while (err > tol)
	{
	    sor_step(&A, u, b, omega);
	    err = res_norm(&A, u, b) / n;
	    k++;
	    if (k > maxit)
	    {
		printf ("not convergent!\n");
		break;
	    }
	}
    }
    else
    {
	printf ("Error! No such mode. 0 = GS, 1 = SOR, 2 = CG, 3 = PCG. \n");
	exit(-1);
    }
    t = clock() - t;
    printf ("The linear solver tooks me %d clicks (%f seconds).\n",(int)t,((float)t)/CLOCKS_PER_SEC);
    printf ("the number of iteration steps: %d\n", k);

	
    t = clock();
    error = 0;
    /// ��ֵ�����
    fprintf (output,"clear\n");
    fprintf (output,"u=[\n");
    delete_pattern(&pA);
    for (j = 0; j <= N + 1; j++)
	fprintf(output,"%lf\t", bnd(X0 + j * h, Y0));
    fprintf(output,"\n");
    for (i = 1; i <= N; i++)
    {
	fprintf(output,"%lf\t", bnd(X0, Y0 + i * h));
	for (j = 1; j <= N; j++)
	{
	    fprintf(output,"%lf\t", u[(i - 1) * N + j - 1]);
	    error += fabs(u[(i - 1) * N + j - 1] - bnd(X0 + j * h, Y0 + i * h)) * fabs(u[(i - 1) * N + j - 1] - bnd(X0 + j * h, Y0 + i * h));
	}
	fprintf(output,"%lf\n", bnd(X1, Y0 + i * h));
    }
    for (j = 0; j <= N + 1; j++)
	fprintf(output,"%lf\t", bnd(X0 + j * h, Y1));
    fprintf (output,"\n");
    fprintf (output,"];\n");
    fprintf (output, "x=linspace(%lf,%lf,%d);\n", X0, X1, N + 2);
    fprintf (output, "y=linspace(%lf,%lf,%d);\n", Y0, Y1, N + 2);
    fprintf (output, "[X Y]=meshgrid(x,y);\n");
    fprintf (output, "surf(X,Y,u);\n");
    t = clock() - t;
    printf ("The outputing tooks me %d clicks (%f seconds).\n",(int)t,((float)t)/CLOCKS_PER_SEC);
    error = sqrt(error) / N / N;
    printf ("error = %f\n", error);
    for (i = 0; i < N * N; i++)
	printf ("u[%d] = %f\n", i, u[i]);
    fclose(output);
    fclose(input);
    free(u);
    free(r);
    free(Ap);
    free(p);
    free(b);
    return 0;
}

double source(double x, double y)
{
    return 2.0 * sin(x) * sin(y);
};

double bnd(double x, double y)
{
    return sin(x) * sin(y);
};

