/*
 * 原 main.cpp（918行）已拆分为多个模块化文件
 */

#include <iostream>
#include "GameSystem.h"

int main() {
    // 设置本地化以支持中文显示
    std::ios::sync_with_stdio(false);
    
    GameSystem::getInstance()->run();
    return 0;
}
