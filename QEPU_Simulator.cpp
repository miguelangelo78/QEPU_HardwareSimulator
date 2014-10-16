#include "QEPU_Lib\stdafx.h"
#include "QEPU_Lib\core\cpu\quantum_cpu\qepu.h"
#include <string>
int main(int argc, char** argv){
	if (argc != 2){ std::cerr << "Usage: "<<argv[0]<<" C:\\...\\filename.bin. Exiting..."; std::cin.get(); exit(-1); }
	
	std::string binaryfile_path=argv[1];
	std::string to_replace = ".qasm";
	binaryfile_path.replace(binaryfile_path.find(to_replace), std::string(to_replace).length(),".bin");
	QEPU q((char*) binaryfile_path.c_str());
	q.run();
	
	return 0;
}