# 基础代数包

我们做计算总是需要一些基础工具的，比如矩阵向量运算。这里有一些事实上的
标准库，比如BLAS, lapack。它们的一个重要衍生产品就是Matlab。不过这些库
都是Fortran编写的。当然也有一些C/C++接口可以用。gsl是GNU基金会提供的一
个用于科学计算的C库，它提供了BLAS和lapack的C接口，并且提供了大量的和计
算有关的数学工具。

gsl的设计目的和早期Matlab有点类似，它为数学家提供了快速实现算法的工具，
在小规模的计算上比较方便，但对并行、系数矩阵和大规模问题支持不是很好。
当然这一情况随着版本提升在慢慢改善。本目录下提供一些gsl计算的例子。此
外，在更复杂的计算中，我们也会将gsl作为基本工具。

## 安装

    sudo apt-get install libgsl-dev gsl-doc-pdf

这样在Ubuntu 18.04下会安装2.4版本。有能力的同学也可以尝试去gnu官网下载
源码自己编译，这样可以得到最新的版本。

## 使用时编译
以目录下test_gsl.cpp为例：

    g++ -o test_gsl test_gsl.cpp -lgsl -lgslcblas


[返回](../README.md)