#include <iostream>

// 计算整数平方的函数
int square(int num) {
    return num * num;
}

int main() {
    int number;

    // 提示用户输入
    std::cout << "请输入一个整数: ";
    std::cin >> number;

    // 计算平方并输出
    int result = square(number);
    std::cout << "您输入的数字的平方是: " << result << std::endl;

    return 0; // 返回 0 表示正常退出
}