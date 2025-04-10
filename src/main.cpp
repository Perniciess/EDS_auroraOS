#include <CryptoKeyManager.hpp>
#include <iostream>


int main() {
	CryptoKeyManager keyManager; 

	std::cout << "Проверка наличия папки с ключами" << std::endl;
	if (keyManager.checkFolder() == 1) {
		keyManager.generateFolder();
	} else {
		std::cout << "Ошибка создания папки с ключами" << std::endl;
		return 1;
	}


	keyManager.checkKeys();
	return 0;
}