#include <iostream>
#include <fstream>
#include <string>
#include "..\..\extern\abulafia_all.h"
#include "generators.h"

int main(int argc, char** argv)
{
	assert(argc == 2);

	std::string filePath(argv[1]);
	std::string ssfnPath = AUTOGEN_DIR_PATH + std::string("\\") + std::string(SHADER_STRUCT_FILE_NAME);
	std::string sbfnPath = AUTOGEN_DIR_PATH + std::string("\\") + std::string(SHADER_BINDINGS_FILE_NAME);

	std::ifstream shaderIn(filePath);
	std::ofstream outSSFN(ssfnPath, std::ios::app);
	std::ofstream outSBFN(ssfnPath, std::ios::app);
	std::string line;

	while (std::getline(shaderIn, line))
	{

	}

	return 0;
}