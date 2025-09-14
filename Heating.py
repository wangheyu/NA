import numpy as np
import matplotlib.pyplot as plt
from scipy.sparse import csr_matrix
from scipy.sparse.linalg import spsolve

# 参数设置
L = 10  # 正方形区域的边长
N = 50  # 网格点数
dx = L / (N - 1)  # 空间步长
dy = dx  # 假设x和y方向的空间步长相等
dt = 0.01  # 时间步长
T = 2  # 总模拟时间
alpha = 1  # 热扩散系数
iterations = int(T / dt)  # 迭代次数

# 创建网格
x = np.linspace(0, L, N)
y = np.linspace(0, L, N)
X, Y = np.meshgrid(x, y)

# 初始条件函数
def initial_condition(X, Y):
    return np.sin(np.pi * X / L) * np.sin(np.pi * Y / L)

# 边界条件函数
def boundary_condition(U):
    U[0, :] = 0
    U[-1, :] = 0
    U[:, 0] = 0
    U[:, -1] = 0
    return U

# 源项函数
def source_term(X, Y, t):
    return 0  # 可以根据需要修改

# 构建系数矩阵A
main_diag = (2 + 2 * alpha * dt / dx**2) * np.ones(N**2)
off_diag = (-alpha * dt / dx**2) * np.ones(N**2 - N)
A = csr_matrix((off_diag, (np.arange(N**2 - N), np.arange(1, N**2))), shape=(N**2, N**2))
A = csr_matrix((main_diag, (np.arange(N**2), np.arange(N**2)))) + A + A.T

# 初始化温度分布
U = initial_condition(X, Y)
U = boundary_condition(U)

# 时间演化
for t in range(1, iterations + 1):
    # 计算源项
    S = source_term(X, Y, t * dt)
    S = S.flatten()
    
    # 构建右侧向量
    b = U.flatten() + dt * S
    
    # 应用边界条件
    b[0:N] = 0
    b[N**2-N+1:N**2] = 0
    b[0::N] = 0
    b[-1::N] = 0
    
    # 解线性系统
    U_new = spsolve(A, b)
    
    # 更新温度分布
    U = U_new.reshape(N, N)
    U = boundary_condition(U)

    # 可视化结果
    if t % 10 == 0:  # 每10个时间步绘制一次
        plt.imshow(U, cmap='viridis', extent=(0, L, 0, L), origin='lower')
        plt.colorbar()
        plt.title(f'Temperature Distribution at Time T={t * dt:.2f}')
        plt.show()

# 最终结果
plt.imshow(U, cmap='viridis', extent=(0, L, 0, L), origin='lower')
plt.colorbar()
plt.title(f'Temperature Distribution at Time T={T:.2f}')
plt.show()