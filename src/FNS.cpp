#include <iostream>
#include <limits>
#include <bitset>
#include <string>
#include <iomanip>
#include <cstdint>
#include <cmath>

// 将二进制字符串转换为浮点数
float Binary2Float(const std::string& binaryStr) {
    // 定义一个联合体，它既可以存储float类型的值，也可以存储uint32_t类型的整数
    union {
        float f; // 联合体的float成员，用于存储浮点数
        uint32_t i; // 联合体的uint32_t成员，用于存储浮点数的二进制整数表示
    } u;
    u.i = std::bitset<32>(binaryStr).to_ulong();
    return u.f;
}

// 将浮点数转换为二进制字符串
std::string Float2Binary(float num) {
    // 定义一个联合体，它既可以存储float类型的值，也可以存储uint32_t类型的整数
    union {
        float f; // 联合体的float成员，用于存储浮点数
        uint32_t i; // 联合体的uint32_t成员，用于存储浮点数的二进制整数表示
    } u;
    u.f = num;
    return std::bitset<32>(u.i).to_string();
}


// 主函数入口
int main() {
    // 探测浮点数的表示范围和精度
    // FPN 常数探测: 
    // 尾数长度 p:
    int p = std::numeric_limits<double>::digits;
    std::cout << "Number of digits in the mantissa (p): " << p << std::endl;

    // 基数 Beta:
    int beta = std::numeric_limits<double>::radix;
    std::cout << "Base of the floating-point representation (Beta): " << beta << std::endl;

    // 指数范围 [L, U]:
    int L = std::numeric_limits<double>::min_exponent - 1;
    int U = std::numeric_limits<double>::max_exponent - 1;
    std::cout << "Exponent range [L, U]: [" << L << ", " << U << "]" << std::endl;

    // Machine epsilon:
    double eps = std::numeric_limits<double>::epsilon();
    std::cout << "Machine epsilon: " << std::setprecision(10) << eps << std::endl;
    std::cout << "Check: eps = 2^(1 - p) = " << std::pow(2.0, 1 - p) << std::endl; 

    // UFL 和 OFL:
    double UFL = std::numeric_limits<double>::min();
    double OFL = std::numeric_limits<double>::max();
    std::cout << "Underflow Level (UFL): " << UFL << std::endl;
    std::cout << "Check: UFL = Beta^L = " << std::pow(beta, L) << std::endl;
    std::cout << "Overflow Level (OFL): " << OFL << std::endl;
    std::cout << "Check: OFL = (Beta - Beta^(1 - p)) * Beta^U = " 
    << (beta - std::pow(beta, 1 - p)) * std::pow(beta, U) << std::endl;

    getchar(); // 暂停，等待用户输入

    // 检查 Over Flow 
    double overflowTest = OFL*(1.0 + eps);
    std::cout << "Overflow Test (OFL*(1.0 + eps)): " << overflowTest << std::endl;
    double underflowTest = UFL*eps;
    std::cout << "Underflow Test (UFL*eps): " << underflowTest << std::endl;
    double tininessUnderflowTest = UFL*eps*0.5;
    std::cout << "Tininess Underflow Test (UFL*eps*0.5): " << tininessUnderflowTest << std::endl;
    double denormMin = std::numeric_limits<double>::denorm_min();
    std::cout << "Denormalized Minimum (denorm_min): " << denormMin << std::endl;

    getchar(); // 暂停，等待用户输入

    // 定义一个float类型的变量，并初始化为3.14f
    float num = 1.0f;

    // 输出提示信息和变量的值
    std::cout << "The binary representation of " << num << " is: " << Float2Binary(num) << std::endl;

    // 给一个 32 位的二进制字符串
    std::string binaryStr = "00111111100000000000000000000000";
    // 输出提示信息和变量的值
    std::cout << "The float representation of " << binaryStr << " is: " << Binary2Float(binaryStr) << std::endl;

    num = 0.5f;        
    std::cout << "The binary representation of " << num << " is: " << Float2Binary(num) << std::endl;

    num = 2.0f;        
    std::cout << "The binary representation of " << num << " is: " << Float2Binary(num) << std::endl;

    // 最大的规格浮点数的 32 位二进制字符串
    binaryStr = "01111111011111111111111111111111";
    std::cout << "The float representation of " << binaryStr << " is: " << (num = Binary2Float(binaryStr)) << std::endl;
    // 和numeric_limits的最大值比较
    std::cout << "Compare to the biggest float in numeric_limits:" << std::numeric_limits<float>::max() << std::endl;
    std::cout << "The difference is: " << std::numeric_limits<float>::max() - num << std::endl;

    // 最接近 0 的正规格浮点数的 32 位二进制字符串
    binaryStr = "00000000100000000000000000000000";
    std::cout << "The float representation of " << binaryStr << " is: " << (num = Binary2Float(binaryStr)) << std::endl;
    std::cout << "Compare to the smallest positive float in numeric_limits:" << std::numeric_limits<float>::min() << std::endl;
    std::cout << "The difference is: " << std::numeric_limits<float>::min() - num << std::endl;

    // 最小的负规格浮点数的 32 位二进制字符串
    binaryStr = "11111111011111111111111111111111";
    std::cout << "The float representation of " << binaryStr << " is: " << (num = Binary2Float(binaryStr)) << std::endl;
    std::cout << "Compare to the smallest positive float in numeric_limits:" << std::numeric_limits<float>::lowest() << std::endl;
    std::cout << "The difference is: " << std::numeric_limits<float>::lowest() - num << std::endl;

    float zero, mzero;
    // 零的 32 位二进制字符串
    binaryStr = "00000000000000000000000000000000";
    std::cout << "The float representation of " << binaryStr << " is: " << (zero = Binary2Float(binaryStr)) << std::endl;
    binaryStr = "10000000000000000000000000000000";
    std::cout << "The float representation of " << binaryStr << " is: " << (mzero = Binary2Float(binaryStr)) << std::endl;

    std::cout << "The difference is: " << zero - mzero << std::endl;

   // 最接近 0 的正非规格浮点数的 32 位二进制字符串
    binaryStr = "00000000000000000000000000000001";
    std::cout << "The float representation of " << binaryStr << " is: " << (num = Binary2Float(binaryStr)) << std::endl;
    std::cout << "Compare to the smallest positive float (denormlized) in numeric_limits:" << std::numeric_limits<float>::denorm_min() << std::endl;
    std::cout << "The difference is: " << std::numeric_limits<float>::denorm_min() - num << std::endl;

    // NaN 的 32 位二进制字符串之一
    binaryStr = "01111111111111111111111111111111";
    std::cout << "The float representation of " << binaryStr << " is: " << (num = Binary2Float(binaryStr)) << std::endl;

    // Inf 的 32 位二进制字符串
    binaryStr = "01111111100000000000000000000000";
    std::cout << "The float representation of " << binaryStr << " is: " << (num = Binary2Float(binaryStr)) << std::endl;

    // -Inf 的 32 位二进制字符串
    binaryStr = "11111111100000000000000000000000";
    std::cout << "The float representation of " << binaryStr << " is: " << (num = Binary2Float(binaryStr)) << std::endl;

    // eps 的 32 位二进制字符串
    binaryStr = "00110100000000000000000000000000";
    std::cout << "The float representation of " << binaryStr << " is: " << (num = Binary2Float(binaryStr)) << std::endl;
    std::cout << "Compare to the smallest positive float in numeric_limits:" << std::numeric_limits<float>::epsilon() << std::endl;
    std::cout << "The difference is: " << std::numeric_limits<float>::epsilon() - num << std::endl;
 
    // 给一个 eps + 1 的 32 位的二进制字符串
    binaryStr = "00111111100000000000000000000001";
    // 输出提示信息和变量的值
    std::cout << binaryStr << " - eps/2 is: " 
    << Float2Binary(Binary2Float(binaryStr) - std::numeric_limits<float>::epsilon() * 0.5) 
    << std::endl;
    
    num = float(2.0 / 3.0);        
    std::cout << "The binary representation of " << num << " is: " << Float2Binary(num) << std::endl;

    return 0;
}