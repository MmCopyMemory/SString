#include <iostream>
#include "SString.h"

int main() {
	constexpr auto enc = OBFSTR("Hello, world!");
	std::cout << enc.reveal() << "\n";
	std::cin.get();
}
