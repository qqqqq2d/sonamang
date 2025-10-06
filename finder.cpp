#include <iostream>
#include <random>
#include <array>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <string>
#include <set>

int main () {

	std::array<std::string, 32> tähed = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "E", "S", "Š", "Z", "Ž", "T", "U", "V", "W", "Õ", "Ä", "Ö", "Ü", "X", "Y"};
	
	
	std::vector<std::string> sobivad_kombinatsioonid{};
	std::set<std::string> kõik_sobivad_sõnad{};
	
label:
	
	std::set<std::string> sobivad_sõnad{};
	
	int kombinatsiooni_pikkus = 3;
	std::string kombinatsioon = "";
	int suvaline_tähe_nr = 0;
	std::string sõna = "";
	std::ifstream sonade_list("estonian.txt");
	

	//Suvalise kombinatsiooni tekitamine		
	for (int i = 0;i < kombinatsiooni_pikkus;i++) {
		
		std::random_device dev;
		std::mt19937 rng(dev());
		std::uniform_int_distribution<std::mt19937::result_type> vahemik1(0,31);
			
		suvaline_tähe_nr = vahemik1(rng);	
			
		kombinatsioon += tähed[suvaline_tähe_nr];
		
	}
		
	std::cout << "kombinatsioon:" << kombinatsioon << std::endl;
		
	std::string vaadeldav_sõna;	
	
	//Vaadetakse sõna sobivust kombinatsiooniga	
	while (std::getline (sonade_list, sõna)) {
	 	vaadeldav_sõna = sõna;
	 	std::transform(vaadeldav_sõna.begin(), vaadeldav_sõna.end(), vaadeldav_sõna.begin(), ::toupper);
	 	
	 	size_t pos = vaadeldav_sõna.find(kombinatsioon);
		if (pos != std::string::npos) {
			sobivad_sõnad.insert(vaadeldav_sõna);
			kõik_sobivad_sõnad.insert(vaadeldav_sõna);
		}		
	}
	
	std::cout << sobivad_sõnad.size() << std::endl;
	if (sobivad_sõnad.size() < 150) {	
		std::cout << "Uuesti proovida" << std::endl;
		goto label;
	}	
		
	sobivad_kombinatsioonid.push_back(kombinatsioon);
	if (sobivad_kombinatsioonid.size() < 10000) {	
		goto label;
	}
	
	//Kirjutada sõnad faili
	std::ofstream sonade_fail("sonad3.txt", std::ios::app);
	for (auto i : kõik_sobivad_sõnad) {
		sonade_fail << i << std::endl;
	}
	sonade_fail.close();
	
	//Väljastada sobivad kombinatsioonid
	std::cout << "Sobivad kombinatsioonid:";
	for (auto i : sobivad_kombinatsioonid) {
        std::cout << i << " ";       
    }
        
    //Kirjutada kombinatsioonid faili
    std::ofstream kombinatsioonide_fail("kombinatsioonid3.txt", std::ios::app);
	for (auto i : sobivad_kombinatsioonid) {
		kombinatsioonide_fail << i << std::endl;
	}
	kombinatsioonide_fail.close();   
  	
  
    return 0;

	
		
	
	
	
	
	
}
