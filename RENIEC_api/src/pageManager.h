//
//  pageManager.h
//  RENIEC_api
//
//  Created by Angello Llerena on 11/11/24.
//
#ifndef PAGEMANAGER_H
#define PAGEMANAGER_H

#include <fstream>
#include <string>
#include <set>
#include "page.h"

using namespace std;

class PageManager {
public:
    PageManager(const std::string& filename);
    ~PageManager();

    uint32_t allocatePage();
    void readPage(uint32_t page_id, Page& page);
    void writePage(uint32_t page_id, const Page& page);

private:
    fstream file;
    string filename;
    set<uint32_t> free_pages;
    uint32_t num_pages;

};

#endif // PAGEMANAGER_H

