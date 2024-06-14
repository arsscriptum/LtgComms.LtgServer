

//==============================================================================
//
//     test.h
//
//============================================================================
//  Copyright (C) 2024 Luminator Tech. Group  <guillaume.plante@luminator.com>
//==============================================================================


#ifndef __TEST_H__
#define __TEST_H__

#include <vector>
#include <string>
#include <memory>

void test_blocking_queue();
uint32_t generateConsecutiveByteBuffer(uint32_t size, std::vector<byte>& vOutData);
uint32_t generateRandomByteBuffer(uint32_t size, std::vector<byte>& vOutData);



#endif //__TEST_H__