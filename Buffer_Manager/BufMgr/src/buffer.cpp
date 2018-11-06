/**
 * @author See Contributors.txt for code contributors and overview of BadgerDB.
 *
 * @section LICENSE
 * Copyright (c) 2012 Database Group, Computer Sciences Department, University of Wisconsin-Madison.
 */

#include <memory>
#include <iostream>
#include "buffer.h"
#include "exceptions/buffer_exceeded_exception.h"
#include "exceptions/page_not_pinned_exception.h"
#include "exceptions/page_pinned_exception.h"
#include "exceptions/bad_buffer_exception.h"
#include "exceptions/hash_not_found_exception.h"

namespace badgerdb { 

BufMgr::BufMgr(std::uint32_t bufs)
	: numBufs(bufs) {
	bufDescTable = new BufDesc[bufs];

  for (FrameId i = 0; i < bufs; i++) 
  {
  	bufDescTable[i].frameNo = i;
  	bufDescTable[i].valid = false;
  }

  bufPool = new Page[bufs];

	int htsize = ((((int) (bufs * 1.2))*2)/2)+1;
  hashTable = new BufHashTbl (htsize);  // allocate the buffer hash table

  clockHand = bufs - 1;
}


BufMgr::~BufMgr() {
	for (FrameId i = 0; i < numBufs; i++) {
		if (bufDescTable[i].dirty == true) {
			//TODO flush page
			//File myFile = bufDescTable[i].file;
			//writePage bufPool[i]
		}
	}
	delete [] bufDescTable;
	delete [] bufPool;
	delete hashTable;
}

void BufMgr::advanceClock()
{
	//TODO do it do?
	if(clockHand >= numBufs - 1){
		clockHand = 0;
	}
	else{
		clockHand++;
	}
}

void BufMgr::allocBuf(FrameId & frame) 
{
	//Make a new free frame using the clock algorithm, if necessary

	bool isFull = true;
	bool allPinned = true;
	for(int i = 0; i < numBufs; i++){
		if(bufDescTable[i].valid == false){
			isFull = false;
		}
		if(bufDescTable[i].pinCnt == 0){
			allPinned = false;
		}
	}

	if(allPinned){
		throw BufferExceededException();
	}

	if(isFull){
		//TODO clock algorithm, evict a page if we can
	}

	//If we kick a valid page out, remove it from the hashtable

}

	
void BufMgr::readPage(File* file, const PageId pageNo, Page*& page)
{

}


void BufMgr::unPinPage(File* file, const PageId pageNo, const bool dirty) 
{
	//TODO
	FrameId theFrameId = 0;
	FrameId* fidPtr = &theFrameId;
	//hashTable->lookup(file, pageNo, fidPtr);

	//If the page is not found...
	if(theFrameId == 0){
		return;
	}

	if(bufDescTable[theFrameId].pinCnt == 0){
		throw PageNotPinnedException(file->filename, pageNo, theFrameId);
	}

	bufDescTable[theFrameId].pinCnt--;

	if(dirty){
		bufDescTable[theFrameId].dirty = dirty;
	}
}

void BufMgr::flushFile(const File* file) 
{
	//Search through the bufTable for pages belonging to the file
	for(FrameId i = 0; i < numBufs; i++){
		if(bufDescTable[i].file == file){
			if(bufDescTable[i].dirty == true){
				//Flush the page to disk and set dirty bit to false
				//TODO
				badgerdb::Page myPage = bufPool[0];
				//file->writePage(bufDescTable[i].pageNo, &myPage);
				bufDescTable[i].dirty = false;
			}
		}
	}
}

void BufMgr::allocPage(File* file, PageId &pageNo, Page*& page) 
{

}

void BufMgr::disposePage(File* file, const PageId PageNo)
{
    
}

void BufMgr::printSelf(void) 
{
  BufDesc* tmpbuf;
	int validFrames = 0;
  
  for (std::uint32_t i = 0; i < numBufs; i++)
	{
  	tmpbuf = &(bufDescTable[i]);
		std::cout << "FrameNo:" << i << " ";
		tmpbuf->Print();

  	if (tmpbuf->valid == true)
    	validFrames++;
  }

	std::cout << "Total Number of Valid Frames:" << validFrames << "\n";
}

}
