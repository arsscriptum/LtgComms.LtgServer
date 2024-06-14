
//==============================================================================
//
//  test.cpp
//
//==============================================================================
//  Copyright (C) 2024 Luminator Tech. Group  <guillaume.plante@luminator.com>
//==============================================================================



#include "stdafx.h"
#include "test.h"
#include "log.h"
#include "blocking_queue.h"
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <wtypes.h>

#include <sstream>
#include <cstdint>
#include <vector>
#include <random>




uint32_t generateRandomByteBuffer(uint32_t size, std::vector<byte>& vOutData)
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

uint32_t generateConsecutiveByteBuffer(uint32_t size, std::vector<byte>& vOutData)
{
	vOutData.clear();
	vOutData.resize(size);

	uint32_t addedElements = 0;
	for (byte& d : vOutData)
	{
		d = static_cast<char>(addedElements);
		addedElements++;
	}
	return addedElements;
}


void test_blocking_queue()
{
	std::vector<byte> myRandomData;

	const uint32_t numElems = 254;
	//bool ok = numElems == generateRandomByteBuffer(numElems, myRandomData);
	bool ok = numElems == generateConsecutiveByteBuffer(numElems, myRandomData);

	COUTC("Created %d random byte values: %s \n", numElems, ok ? "success" : "error");

	BlockingQueue<byte> _rcvQueue;
	for (int i = 0; i < numElems; i++) {
		printf("EnQ %d\n", myRandomData[i]);
		_rcvQueue.enQ(myRandomData[i]);
	}

	byte v = _rcvQueue.front();
	printf("Front if %d\n", v);
	while (_rcvQueue.size()) {
		printf("Front if %d\n", _rcvQueue.front());
		_rcvQueue.deQ();
	}

}
