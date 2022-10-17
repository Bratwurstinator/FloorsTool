#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <map>

void getMaterials(std::string path, std::vector<std::vector<std::string>> &materials){
	std::ifstream file(path);
	std::string line;
	std::string parsed;
	while (std::getline(file, line))
	{
		std::vector<std::string> material;
		for(int i = 0; i <= line.length(); i++){
			if(line[i] == '/'){
				material.push_back(parsed);
				goto COMMENT; //ignore comments
			}
			if(line[i] == ';' || i == line.length()){
				material.push_back(parsed);
				parsed = "";
			}else if(line[i] != ' '){
				parsed += line[i];
			}
		}
		COMMENT:
		materials.push_back(material);
	}
}

void makeFloors(std::string path, std::vector<std::vector<std::string>> &materials){
	std::ifstream file(path);
	std::string line;
	std::string parsed;
	std::string filename;
	std::string defType = "";
	while (std::getline(file, line))
	{
		size_t pos, pos2, pos3;
		if(defType == "" && (pos = line.find("<")) != std::string::npos){ //Identify uppermost node. Could use an XML parser but those are slooow.
			pos2 = line.find(" ", pos);
			pos3 = line.find(">", pos);
			defType = line.substr(pos+1, (pos2 < pos3 ? pos2-pos-1 : pos3-pos-1));
			std::cout << defType << " " << pos << " "<< pos2 << " " << pos3 << std::endl;
		}
		if(defType != ""){
			if(line.find("<"+defType) != std::string::npos){
				parsed = "";
			}
			if(line.find("<defName>") != std::string::npos){
				filename = line;
				int pos, npos;
				while((pos = filename.find("[")) != std::string::npos && (npos = filename.find("]", pos)) != std::string::npos ){filename.replace(pos, npos-pos+1, "");}
				while((pos = filename.find("<")) != std::string::npos && (npos = filename.find(">", pos)) != std::string::npos ){filename.replace(pos, npos-pos+1, "");}
				while((pos = filename.find(" ")) != std::string::npos){filename.replace(pos, 1, "");} //Extract filename. Possible via regex but nah
				filename += "_MaterialDefs.xml";
			}
			parsed += line + "\n";
			if(line.find("</"+defType+">") != std::string::npos){
				std::ofstream output(filename);
				output << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>" << std::endl << "<Defs>" << std::endl;
					for(int i = 1; i < materials.size(); i++){
						int pos;
						std::string parsed2 = parsed;
						for(int j = 0; j < materials[i].size(); j++){ //create new def per material
							while((pos = parsed2.find(materials[0][j])) != std::string::npos){parsed2.replace(pos, materials[0][j].length(), materials[i][j]);}
						}
						output << std::endl << parsed2;
					}
				output << std::endl << "</Defs>";
				defType = "";
			}
		}
	}
	
}



int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		std::cout << "not enough arguments - USAGE: floors.exe materials.txt floors.txt\n";
		return -1; // invalid number of parameters
	}

    std::vector<std::vector<std::string>> materials = std::vector<std::vector<std::string>>();
	getMaterials(argv[1], materials);
	makeFloors(argv[2], materials);
	return 0;
}
