#include <iostream>
#include <string>
#include <fstream>
#include <random>

int main() {
				
	std::string sõna;
	int muutuv_rida = 0;
	int ridade_arv = 282173;
	
	std::string suvaline_sonõ;
	std::ifstream sonade_list("estonian.dict");
	
	/*while (std::getline (sonade_list, sona)) {
 		
	}*/
	
	std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> suvaline_rida(0,ridade_arv);
	
	int rida = suvaline_rida(rng);
	
	while (std::getline (sonade_list, sõna)) {
 		if (muutuv_rida == rida)
 			std::cout << sõna << std::endl;
 		muutuv_rida++;
	}
			
	sonade_list.close();
}
