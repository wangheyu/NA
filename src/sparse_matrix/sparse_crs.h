/**
 * @file   sparse_crs.h
 * @author WANG Heyu <hywang@x200t>
 * @date   Tue Oct 10 08:39:32 2017
 * 
 * @brief  ϡ�����ģ��ʵ��.
 * 
 * 
 */

#ifndef _SPARSE_CRS_WHY_
#define _SPARSE_CRS_WHY_
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/**
 * ϡ����ģ��.
 * 
 */
struct PATTERN 
{
    int n;			/**< ����. */
    int m;			/**< ����. */
    int *row;			/**< ��ָ��. */
    int *col;			/**< ��ָ��. */
    int is_compressed;		/**< �Ƿ�ѹ��. */
    struct node *registed;	/**< ���ø�ģ��ľ���. */
};

/**
 * ϡ�����.
 * 
 */
struct MATRIX
{
    struct PATTERN *pat;	/**< ģ��ָ��. */
    double *ele;		/**< ����Ԫ. */
};

/**
 * ����ע��ʹ��ģ�����Ľڵ�. ��ÿ��ģ���ж���¼��ʹ�����ľ���, ����
 * һ������.
 * 
 */
struct node
{
    struct node *next;
    struct MATRIX *mat;
};

/** 
 * ��ʼ��ģ��.
 * 
 * @param pattern ģ��ָ��.
 * @param n ����.  		   
 * @param m ����.  		   
 * @param nzmax ÿ��������Ԫ����. 
 * 
 * @return �������.
 */
int init_pattern(struct PATTERN *pattern, int n, int m, int nzmax);

/** 
 * ��ʾģ����Ϣ. 
 * 
 * @param pattern ģ��ָ��.
 * 
 * @return �������.
 */
int print_pattern(const struct PATTERN *pattern);

/** 
 * ɾ��ģ��. ע�������ɾ��ȫ��ʹ�ø�ģ��ľ����е�Ԫ��, �Ա����ڴ�й
 * ©. ��˲����ṩ�����ľ���ɾ������.
 * 
 * @param pattern ģ��ָ��.
 * 
 * @return �������.
 */
int delete_pattern(struct PATTERN *pattern);

/** 
 * ����ģ�����Ԫ.
 * 
 * @param pattern ģ��ָ��.
 * @param pi �б�.
 * @param pj �б�.
 * 
 * @return �������.
 */
int set_pattern(struct PATTERN *pattern, int pi, int pj);

/** 
 * ѹ��ģ��.
 * 
 * @param pattern ģ��ָ��. 
 * 
 * @return �������.
 */
int compress_pattern(struct PATTERN *pattern);

/** 
 * ��ʼ������.
 * 
 * @param mat ����ָ��.
 * @param pattern ģ��ָ��.
 * 
 * @return �������.
 */
int init_matrix(struct MATRIX *mat, struct PATTERN *pattern);

/** 
 * ��ʾ������Ϣ.
 * 
 * @param mat ����ָ��.
 * 
 * @return �������.
 */
int print_matrix(const struct MATRIX *mat);

/** 
 * ��ȡ mat[ai, aj] ����ֵ. �������Ԫ�򷵻� 0.
 * 
 * @param mat ����.
 * @param ai �б�.
 * @param aj �б�.
 * 
 * @return mat[ai, aj].
 */
double read_matrix(const struct MATRIX *mat, int ai, int aj);

/** 
 * ���Ӿ���Ԫ��. �� e �ӵ� mat[ai, aj] ��.
 * 
 * @param mat ����ָ��.
 * @param ai �б�.
 * @param aj �б�.
 * @param e ����Ԫ.
 * 
 * @return �������.
 */
int add_matrix(struct MATRIX *mat, int ai, int aj, double e);

/** 
 * �����������.
 * 
 * @param mat ����ָ��.
 * @param iv ������ָ��.
 * @param ov ������ָ��.
 * 
 * @return 
 */
int mat_m_vec(const struct MATRIX *mat, const double *iv, double *ov);

/** 
 * һ�� GS ����.
 * 
 * @param mat ϵ������. 
 * @param x δ֪��.
 * @param b �Ҷ���.
 * 
 * @return 
 */
int gs_step(const struct MATRIX *mat, double *x, const double *b);

/** 
 * һ�� SOR ������
 * 
 * @param mat ϵ������.
 * @param x δ֪��.
 * @param b �Ҷ���.
 * @param w �ɳ�����.
 * 
 * @return 
 */
int sor_step(const struct MATRIX *mat, double *x, const double *b, double w);

/** 
 * ����ϵͳ��������.
 * 
 * @param mat ϵ������.
 * @param x δ֪��.
 * @param b �Ҷ���.
 * 
 * @return norm(Ax - b), 2-��.
 */
double res_norm(const struct MATRIX *mat, const double *x, const double *b);

/** 
 * ����ϵͳ������.
 * 
 * @param mat ϵ������.
 * @param x δ֪��.
 * @param b �Ҷ���.
 * @param r ���ز���.
 * 
 * @return �������.
 */
int res(const struct MATRIX *mat, const double *x, const double *b, double *r);

/** 
 * ��λԤ����, �൱�ڲ�Ԥ����, ���ڵ���. ʵ�� w = I^-1 * v.
 * 
 * @param mat ����ϵͳϵ������, ����Ԥ������� M!
 * @param v = I * w
 * @param w = I^-1 * v
 * 
 * @return �������.
 */
int pc_id(const struct MATRIX *mat, const double *v, double *w);

/** 
 * SSOR Ԥ����, ʵ�� w = M_ssor^-1 * v.
 * 
 * @param mat ����ϵͳϵ������, ����Ԥ������� M!
 * @param v = M_ssor * w 
 * @param w = M_ssor^-1 * v
 * @param omg �ɳ�����. 
 * 
 * @return �������.
 */
int pc_ssor(const struct MATRIX *mat, const double *v, double *w, double omg);

/** 
 * ����ȫ Cholesky �ֽ�Ԥ���� IM0.  
 * 
 * @param mat ����ϵͳϵ������, ����Ԥ������� M!
 * @param v = M * w; 
 * @param w = M^-1 * v.
 * 
 * @return �������.
 */
int pc_im0(const struct MATRIX *mat, const double *v, double *w);


#else
/// do nothing
#endif
