#include <string>
#include <iostream>
#include <chrono>
#include <thread>

//acronimo di printTextEffect (standard: 1,250)
void PTE(std::string s, int delayShort = 1, int delayLong = 250) {
    for (int i = 0; i < s.size(); i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(delayShort));
        std::cout << s[i];
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(delayLong));
    std::cout << std::endl;
}