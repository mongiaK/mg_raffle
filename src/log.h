/*================================================================
*  
*  文件名称：log.h
*  创 建 者: mongia
*  创建日期：2021年06月03日
*  
================================================================*/

#pragma once

#include <iostream>

#define slog_error(x) std::cerr << x << std::endl;
#define slog_info(x) std::cout << x << std::endl;
#define slog_debug(x) std::cout << x << std::endl;
#define slog_warn(x) std::cout << x << std::endl;

