#include <iostream>
#include <vector>
#include <fstream>
#include <random>
#include <stdlib.h>
#include <algorithm>

int main() {
	
	std::vector<std::string> sõnad{};
	std::vector<std::string> kombinatsioonid{};
	
	//Lugeda sõnad listist
	std::string sõna;
	std::ifstream sonade_list("sonad.txt");
	
	while (std::getline (sonade_list, sõna)) {
		sõnad.push_back(sõna);
	}
	
	//Lugeda kombinatsioonid listist
	std::string kombinatsioon;
	std::ifstream kombinatsioonide_list("kombinatsioonid.txt");
	
	while (std::getline (kombinatsioonide_list, kombinatsioon)) {
		kombinatsioonid.push_back(kombinatsioon);
	}
	
	//Mängu loop
	
	int suvaline_kombinatsioon;
	int elud = 3;
	bool õige_vastus = false;

	while (true) {
	
		std::string vastus = "";
		
		std::random_device dev;
		std::mt19937 rng(dev());
		std::uniform_int_distribution<std::mt19937::result_type> kombinatsiooni_vahemik(0,kombinatsioonid.size());
		suvaline_kombinatsioon = kombinatsiooni_vahemik(rng);
		
		std::cout << std::endl << kombinatsioonid[suvaline_kombinatsioon] << std::endl;
		//std::cout << suvaline_kombinatsioon << std::endl;
		
		if (elud > 0) {
			
			õige_vastus = false;
			std::cout << "\nElud: " << elud << std::endl;
		
			std::cout << ">> ";
			std::cin >> vastus;
			std::transform(vastus.begin(), vastus.end(), vastus.begin(), ::toupper);
		
			for (auto i : sõnad) {
				if (i == vastus) {
					std::cout << std::endl << "\033[1;32mÕIGE\033[0m\n";
					õige_vastus = true;
					
					sõnad.erase(std::remove(sõnad.begin(), sõnad.end(), vastus), sõnad.end());
				}
			}
			
			if (õige_vastus == false) {
				elud--;
				õige_vastus = true;
			}
		
		}
		else {
			exit(0);
		}
		
		//break;
		
	}
}
