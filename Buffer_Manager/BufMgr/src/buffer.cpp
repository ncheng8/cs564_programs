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
	// holds the actual pages; no deletions necessary, handled by valid bit
  bufPool = new Page[bufs];

	int htsize = ((((int) (bufs * 1.2))*2)/2)+1;
	// gets frameId from file and pageNo.
  hashTable = new BufHashTbl (htsize);  // allocate the buffer hash table

  clockHand = bufs - 1;
}


BufMgr::~BufMgr() {
	for (FrameId i = 0; i < numBufs; i++) {
		BufDesc currFrame = bufDescTable[i];
		if (currFrame.dirty == true) {
			//flushes dirty pages
			File* myFile = currFrame.file;
			//bufPool at i holds page referenced at bufDescTable at i
			myFile->writePage(bufPool[i]);
		}
	}
	delete [] bufDescTable;
	delete [] bufPool;
	delete hashTable;
}

void BufMgr::advanceClock()
{
	if(clockHand >= numBufs - 1){
		clockHand = 0;
	}	else {
		clockHand++;
	}
}

void BufMgr::allocBuf(FrameId & frame) 
{
	//Make a new free frame using the clock algorithm, if necessary

	bool allPinned = true;
	for(FrameId i = 0; i < numBufs; i++){

		// if(bufDescTable[i].valid == false){
		// 	isFull = false;
		// } probably don't need this

		if(bufDescTable[i].pinCnt == 0){
			allPinned = false;
		}
	}

	if(allPinned){
		throw BufferExceededException();
	}

	bool kickedOutValid = false;
	Page* removedPage;

	//Clock algorithm, evict a page if we can
	bool foundSpace = false;
	while(!foundSpace){
		if(bufDescTable[clockHand].valid){
			
		}
		else{
			frame = clockHand;
			foundSpace = true;
		}
	}

	//If we kick a valid page out, remove it from the hashtable
	if(kickedOutValid){
		hashTable->remove(bufDescTable[clockHand].file, removedPage->page_number());
	}	
}

	
void BufMgr::readPage(File* file, const PageId pageNo, Page*& page)
{
	// if page is not in buffer pool, call allocBuf() to get a frame and read
	// the page from the file into the frame, put page in hash table, set up frame
	// if page is in buffer pool, increment pinCnt and return pointer to its frame
	FrameId frameNum = numBufs + 1;
	try{
		hashTable->lookup(file, pageNo, frameNum);
	}
	catch(HashNotFoundException){
		//case 1
		allocBuf(frameNum);

		badgerdb::Page myPage = file->readPage(pageNo);
		bufPool[frameNum] = myPage;
		bufDescTable[frameNum].Set(file, pageNo);
		page = &bufPool[frameNum];
		return;
	}
	//case 2
	bufDescTable[frameNum].refbit = true;
	bufDescTable[frameNum].pinCnt++;
	page = &bufPool[frameNum];
}


void BufMgr::unPinPage(File* file, const PageId pageNo, const bool dirty) 
{
	FrameId frameNum = numBufs + 1;
  try{
		hashTable->lookup(file, pageNo, frameNum);
	}
	catch(HashNotFoundException){
	  //If the page is not found...
		return;
	}

	if(bufDescTable[frameNum].pinCnt == 0){
		throw PageNotPinnedException(file->filename(), pageNo, frameNum);
	}

	bufDescTable[frameNum].pinCnt--;

	if(dirty){
		bufDescTable[frameNum].dirty = dirty;
	}
}

void BufMgr::flushFile(const File* file) 
{
	//Search through the bufTable for file pages that are not flushable
	for(FrameId i = 0; i < numBufs; i++){
		BufDesc currFrame = bufDescTable[i];		
		if(currFrame.file == file){
			if (currFrame.pinCnt > 0) {
				throw PagePinnedException(currFrame.file->filename(), currFrame.pageNo, currFrame.frameNo);
			}
			if (currFrame.valid == false) {
				throw BadBufferException(currFrame.frameNo, currFrame.dirty, currFrame.valid, currFrame.refbit);
			}
		}
	}
	
	//Search through the bufTable for pages belonging to the file
	for(FrameId i = 0; i < numBufs; i++){
		BufDesc currFrame = bufDescTable[i];		
		if(currFrame.file == file){
			if(currFrame.dirty == true){
				//Flush the page to disk and set dirty bit to false
				FrameId frameNum = numBufs + 1;		
				try{
					hashTable->lookup(currFrame.file, currFrame.pageNo, frameNum);
				}
				catch(HashNotFoundException){
					// should never happen due to previous checks
					return;
				}

				Page myPage = bufPool[frameNum];
				PageId pageNo = currFrame.pageNo;
				currFrame.file->writePage(myPage);
				currFrame.dirty = false;			
			}
			//remove from hashtable
			hashTable->remove(currFrame.file, currFrame.pageNo);
			//invoke clear() of bufdesc
			currFrame.Clear();
		}
	}
}

void BufMgr::allocPage(File* file, PageId &pageNo, Page*& page) 
{
	// allocate an empty page in the file
	badgerdb::Page new_page = file->allocatePage();
	// calls allocBuf() to get a new frame
	FrameId frameNum = numBufs + 1;
	allocBuf(frameNum);
	// insert the entry into the hashtable and call Set()
	hashTable->insert(file, new_page.page_number(), frameNum);
	bufDescTable[frameNum].Set(file, new_page.page_number());

	//place page into pagetable ???
	bufPool[frameNum] = new_page;

	//set pageno and Pageptr
	pageNo = new_page.page_number();
	page = &bufPool[frameNum];
}

void BufMgr::disposePage(File* file, const PageId PageNo)
{
	// if it is in a frame, free the frame and delete it from the hash table
	// call deletePage()		
	FrameId frameNum = numBufs + 1;
	try{
		hashTable->lookup(file, PageNo, frameNum);
	}
	catch(HashNotFoundException){
		return;
	}

	bufDescTable[frameNum].Clear();
	hashTable->remove(file, PageNo);
	file->deletePage(PageNo);
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
