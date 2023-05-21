#include "page_cache_lru_2.hpp"
#include "utilities/exception.hpp"
#include <list>


LRU2ReplacementPageCache::LRU2ReplacementPage::LRU2ReplacementPage(
    int argPageSize, int argExtraSize, unsigned argPageId, bool argPinned)
    : Page(argPageSize, argExtraSize), pageId(argPageId), pinned(argPinned) {}

LRU2ReplacementPageCache::LRU2ReplacementPageCache(int pageSize, int extraSize)
    : PageCache(pageSize, extraSize) {
  // TODO: Implement.

  //Initialize member variables
  maxNumPages_ = 0;
  numFetches_ = 0;
  numHits_ = 0;

  // Initialize lruList_
  lru2List_.clear();
  lru1List_.clear();

  // Initialize pages_
  pages_.clear();

  //throw NotImplementedException("LRU2ReplacementPageCache::LRU2ReplacementPageCache");
}

void LRU2ReplacementPageCache::setMaxNumPages(int maxNumPages) {
  // Set the maximum number of pages in the cache.
  maxNumPages_ = maxNumPages;
  //printf("Max pages: %d", maxNumPages_);

  while (pages_.size() > maxNumPages_) {
    bool unpinnedPage = false;
    if (!lru2List_.empty()) {

      auto& page = lru2List_.front();
      if (!page->pinned) {
        lru2List_.pop_front();
        pages_.erase(page->pageId);
        unpinnedPage = true;
      }
    }

    if (!unpinnedPage && !lru1List_.empty()) {
      auto& page = lru1List_.front();
      if (!page->pinned) {
        lru1List_.pop_front();
        pages_.erase(page->pageId);
        unpinnedPage = true;
      }
    }
    if (!unpinnedPage) {
      break;
    }
  }
}

int LRU2ReplacementPageCache::getNumPages() const {
  // TODO: Implement.
  return (int)pages_.size();
  //throw NotImplementedException("LRU2ReplacementPageCache::getNumPages");
}

Page *LRU2ReplacementPageCache::fetchPage(unsigned int pageId, bool allocate) {

  // increment the number of fetches
  ++numFetches_;
  // Try to find the page in the cache
  auto found = pages_.find(pageId);
  if (found == pages_.end()) {
    // Page not in cache
    if (!allocate) {
      // Page not found and not allowed to allocate
      return nullptr;
    }

    // If the number of pages in the cache is less than the maximum, allocate and return a pointer to a new page.
    if (pages_.size() >= maxNumPages_) {
      // Cache is full, evict a page
      if (lru2List_.empty()) {
        // Second list is empty, cache is pinned
        return nullptr;
      }

      // If all pages in lru2List_ are pinned, return nullptr
      if (std::all_of(lru2List_.begin(), lru2List_.end(), [](auto &page) { return page->pinned; })) {
        return nullptr;
      }
      
      // Remove the least recently used page from the end of the second list
      auto evictedPage = lru2List_.back();
      //auto front1 = lru1List_.front();
      //if(evictedPage->pageId == front1->pageId){
      //  printf("HI");
      //  evictedPage = lru2List_.front();
      //}
      // FOR TESTING
      
      pages_.erase(evictedPage->pageId);
      lru1List_.remove(evictedPage);
      lru2List_.remove(evictedPage);
    }

    // Allocate a new page and insert into the first list
    auto newPage = std::make_unique<LRU2ReplacementPage>(pageSize_, extraSize_, pageId, false);
    LRU2ReplacementPage *result = newPage.get();
    pages_[pageId] = std::move(newPage);
    lru1List_.push_front(result);
    result->lruCounter1 = 1;
    // result->pinned = true;
    result->pinned = true;
    return result;
  }

  // Page found in cache
  LRU2ReplacementPage *result = static_cast<LRU2ReplacementPage *>(found->second.get());
  ++numHits_;

  if (!result->pinned) {
    // Page is not pinned, move to front of first list and increment LRU-1 counter
    lru1List_.remove(result);
    lru1List_.push_front(result); 
    result->lruCounter1++;
  } else {
    result->lruCounter2++; // Increment LRU-2 counter if the page is pinned
  }

  if (result->lruCounter1 > 1) {
    // Page has been accessed twice, move to second list
    lru1List_.remove(result);
    lru2List_.push_front(result); 
    result->lruCounter1 = 0;
    result->lruCounter2 = 1;
  } else if (result->lruCounter2 >= 2) {
    // Page is in second list, move to front and increment LRU-2 counter
    lru2List_.remove(result);
    lru2List_.push_front(result); 
    result->lruCounter2++;
  }
  result->pinned = false;
  

  return result;
}

void LRU2ReplacementPageCache::unpinPage(Page *page, bool discard) {
  auto *lru2Page = (LRU2ReplacementPage *) page;

  if (discard || (pages_.size() > maxNumPages_ && !lru2Page->pinned)) {
    // If discard is true or the number of pages in the cache is greater than the maximum and the page is not pinned, discard the page.
    pages_.erase(lru2Page->pageId);
    lru2List_.remove(lru2Page);
    lru1List_.remove(lru2Page);
  } else {
    if (std::find(lru1List_.begin(), lru1List_.end(), lru2Page) != lru1List_.end()) {
      // If the page is in the first list, remove it and insert it into the second list
      lru1List_.remove(lru2Page);
      lru2List_.push_front(lru2Page); 
      lru2Page->lruCounter1 = 0;
      lru2Page->lruCounter2 = 1;
    }
    lru2Page->pinned = false;
  }
}

void LRU2ReplacementPageCache::changePageId(Page *page, unsigned int newPageId) {
  // TODO: Implement.
  auto *lru2Page = (LRU2ReplacementPage *) page;
  // If the new page ID is the same as the old one, return early.
  if (lru2Page->pageId == newPageId) {
    return;
  }
  // If a page with the new page ID already exists, assume it's unpinned and discard it.
  if (pages_.count(newPageId) > 0) {
    unpinPage(pages_[newPageId].get(), true);
  }
  // Update the page ID and the hash table entry.
  unsigned int oldPageId = lru2Page->pageId;
  lru2Page->pageId = newPageId;
  pages_[newPageId] = std::move(pages_[oldPageId]);
  pages_.erase(oldPageId);
  //throw NotImplementedException("LRU2ReplacementPageCache::changePageId");
}

void LRU2ReplacementPageCache::discardPages(unsigned int pageIdLimit) {
  // TODO: Implement.
  
  auto it = pages_.begin();
  while (it != pages_.end()) {
    if (it->first >= pageIdLimit) {
      // Check if the page is pinned
      if (it->second->pinned) {
        // If the page is pinned, unpin it
        it->second->pinned = false;
      }
      // Discard the page from both lists if it's not pinned
      if (!it->second->pinned) {
        lru2List_.remove(it->second.get());
        lru1List_.remove(it->second.get()); //maybe
        it = pages_.erase(it);
      } else {
        ++it;
      }
    } else {
      ++it;
    }
  }
  //throw NotImplementedException("LRU2ReplacementPageCache::discardPages");
}
