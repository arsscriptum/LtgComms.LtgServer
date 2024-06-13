
//==============================================================================
//
//     main.cpp
//
//============================================================================
//  Copyright (C) 2024 Luminator Tech. Group  <guillaume.plante@luminator.com>
//==============================================================================



#include "stdafx.h"
#include "win32.h"
#include "cmdline.h"
#include "Shlwapi.h"
#include "log.h"
#include "blocking_queue.h"

#include <codecvt>
#include <locale>
#include <vector>
#include <unordered_map>
#include <iterator>
#include <regex>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <cstdint>
#include <vector>
#include <random>


using namespace std;

#pragma message( "Compiling " __FILE__ )
#pragma message( "Last modified on " __TIMESTAMP__ )

void banner();
void usage();

int main(int argc, TCHAR** argv, TCHAR envp)
{

#ifdef UNICODE
	const char** argn = (const char**)C::Convert::allocate_argn(argc, argv);
#else
	char** argn = argv;
#endif // UNICODE

	CmdLineUtil::getInstance()->initializeCmdlineParser(argc, argn);

	CmdlineParser* inputParser = CmdLineUtil::getInstance()->getInputParser();

	CmdlineOption cmdlineOptionHelp({ "-h", "--help" }, "display this help");
	CmdlineOption cmdlineOptionVerbose({ "-v", "--verbose" }, "verbose output");
	CmdlineOption cmdlineOptionPath({ "-p", "--path" }, "path");
	CmdlineOption cmdlineOptionNoBanner({ "-n", "--nobanner" }, "no banner");
	CmdlineOption cmdlineOptionWhatIf({ "-w", "--whatif" }, "whatif");

	inputParser->addOption(cmdlineOptionHelp);
	inputParser->addOption(cmdlineOptionVerbose);
	
	inputParser->addOption(cmdlineOptionNoBanner); 
	inputParser->addOption(cmdlineOptionWhatIf); 
	inputParser->addOption(cmdlineOptionPath); 


	bool optHelp = inputParser->isSet(cmdlineOptionHelp);
	bool optVerbose = inputParser->isSet(cmdlineOptionVerbose);
	bool optPath= inputParser->isSet(cmdlineOptionPath);
	bool optNoBanner = inputParser->isSet(cmdlineOptionNoBanner);
	bool optWhatIf = inputParser->isSet(cmdlineOptionWhatIf);
	if(optNoBanner == false){
		banner();
	}
	if(optHelp){
		usage();
		return 0;
	}

	

	return 0;
}

uint32_t generateRandomByteBuffer(uint32_t size, std::vector<byte>&vOutData)
{
	std::random_device rd;
	std::uniform_int_distribution<uint32_t> dist(0, 0xFFFFFFFFu);

	vOutData.clear();
	vOutData.resize(size);
	
	int offset = 0;
	uint32_t bits = 0;
	uint32_t addedElements = 0;
	for (byte& d : vOutData)
	{
		if (offset == 0)
			bits = dist(rd);
		d = static_cast<char>(bits & 0xFF);
		bits >>= 8;
		if (++offset >= 4)
			offset = 0;

		addedElements++;
	}
	return addedElements;
}

void TestQueue()
{
	std::vector<byte> myRandomData;
	BlockingQueue<byte> _rcvQueue;
	for (int i = 0; i < 1024; i++) {

	}
	enQ()

}



void banner() {
	std::wcout << std::endl;
	COUTC("LtgServer v2.1 - LtgComms Test Utilities\n");
	COUTC("Copyright (C) 2000-2021 Guillaume Plante\n");
	std::wcout << std::endl;
}
void usage() {
	COUTCS("Usage: LtgServer [-h][-v][-n][-p] path \n");
	COUTCS("   -v          Verbose mode\n");
	COUTCS("   -h          Help\n");
	COUTCS("   -p          Port Number (default 9805)\n");
	COUTCS("   -n          No banner\n");
	COUTCS("   -p path     Destination path\n");
	std::wcout << std::endl;
}


