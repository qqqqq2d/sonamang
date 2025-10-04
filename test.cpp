#include <iostream>
#include <vector>
#include <fstream>

int main() {

	std::string sõna;
	std::ifstream sonade_list("sonad.txt");

	std::string kombinatsioon;
	std::ifstream kombinatsioonide_list("kombinatsioonid.txt");

	std::vector<std::vector<std::string>> kombinatsioonid_sõnad;
	
	while (std::getline (kombinatsioonide_list, kombinatsioon)) {
		kombinatsioonid_sõnad.push_back(kombinatsioon);
	}

	for (int i : kombinatsioonid_sõnad) {
		size_t pos = vaadeldav_sõna.find(kombinatsioon);
		std::cout << i << std::endl;
	}

}
