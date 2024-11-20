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
#include <map>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/map.hpp>
#include "page.h"

using namespace std;

class PageManager {
public:
    PageManager(const std::string& filename);
    ~PageManager();

    uint32_t allocatePage();
    bool readPage(uint32_t page_id, Page& page);
    bool writePage(uint32_t page_id, const Page& page);
    
    void savePageIndex();
    void loadPageIndex();

private:
    fstream file;
    string filename;
    std::map<uint32_t, uint64_t> page_offsets;
    set<uint32_t> free_pages;
    uint32_t num_pages;

};

#endif // PAGEMANAGER_H

