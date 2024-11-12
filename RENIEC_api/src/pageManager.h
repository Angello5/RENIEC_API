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
#include "Page.h"

class PageManager {
public:
    PageManager(const std::string& filename);
    ~PageManager();

    size_t allocatePage();
    void readPage(size_t page_id, Page& page);
    void writePage(size_t page_id, const Page& page);

private:
    std::fstream file;
    std::string filename;
    std::set<size_t> free_pages; 
    size_t num_pages;

};

#endif // PAGEMANAGER_H

