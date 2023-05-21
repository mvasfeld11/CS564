#include "page_cache_lru.hpp"
#include "utilities/exception.hpp"
#include <list>
#include <iostream>

LRUReplacementPageCache::LRUReplacementPage::LRUReplacementPage(
    int argPageSize, int argExtraSize, unsigned argPageId, bool argPinned)
    : Page(argPageSize, argExtraSize), pageId(argPageId), pinned(argPinned) {}

LRUReplacementPageCache::LRUReplacementPageCache(int pageSize, int extraSize)
    : PageCache(pageSize, extraSize) {
  // TODO: Implement.

  //Initialize member variables
  maxNumPages_ = 0;
  numFetches_ = 0;
  numHits_ = 0;

  // Initialize lruList_
  lruList_.clear();

  // Initialize pages_
  pages_.clear();

    //throw NotImplementedException(
    //   "LRUReplacementPageCache::LRUReplacementPageCache");
}
  /**
   * Set the maximum number of pages in the cache. Discard unpinned pages until
   * either the number of pages in the cache is less than or equal to
   * `maxNumPages` or all the pages in the cache are pinned. If there are still
   * too many pages after discarding all unpinned pages, pages will continue to
   * be discarded after being unpinned in the `unpinPage` function.
   * @param maxNumPages Maximum number of pages in the cache.
   */
void LRUReplacementPageCache::setMaxNumPages(int maxNumPages) {

  // Set the maximum number of pages in the cache.
  maxNumPages_ = maxNumPages;

  // Calculate the number of pages to remove.
  int numPagesToRemove = std::max(0, static_cast<int>(pages_.size()) - maxNumPages_);

  // Iterate through the LRU list and remove unpinned pages until either the
  // number of pages in the cache is less than or equal to maxNumPages or all
  // pages in the cache are pinned.
  auto it = lruList_.begin();
  while (it != lruList_.end()) {
    if (numPagesToRemove <= 0) {
      break;
    }
    if (!(*it)->pinned) {
      pages_.erase((*it)->pageId);
      it = lruList_.erase(it);
      numPagesToRemove--;
    } else {
      ++it;
    }
  }
}
  // throw NotImplementedException("LRUReplacementPageCache::setMaxNumPages");

  /**
   * Get the number of pages in the cache, both pinned and unpinned.
   * @return Number of pages in the cache.
   */
int LRUReplacementPageCache::getNumPages() const {

  return (int)pages_.size();

  //throw NotImplementedException("LRUReplacementPageCache::getNumPages");
}
  /**
   * Fetch and pin a page. If the page is already in the cache, return a
   * pointer to the page. If the page is not already in the cache, use the
   * `allocate` parameter to determine how to proceed. If `allocate` is false,
   * return a null pointer. If `allocate` is true, examine the number of pages
   * in the cache. If the number of pages in the cache is less than the maximum,
   * allocate and return a pointer to a new page. If the number of pages in the
   * cache is greater than or equal to the maximum, return a pointer to an
   * existing unpinned page. If all pages are pinned, return a null pointer.
   * @param pageId Page ID.
   * @param allocate Allocate new page on miss.
   * @param hit True if the request was a hit and false otherwise.
   * @return Pointer to a page. May be null.
   */
Page *LRUReplacementPageCache::fetchPage(unsigned int pageId, bool allocate) {
  // std::cout << "allocate when fetchPage is called: " << allocate << std::endl;
  ++numFetches_;
  auto found = pages_.find(pageId);
  
  if (found == pages_.end()) {
    // Page not in cache
    // std::cout << "allocate if page not in cache: " << allocate << std::endl;
    if (!allocate) {
      // std::cout << "should be going in here since allocate is false: " << allocate << std::endl;
      return nullptr;
    }
    if (pages_.size() >= maxNumPages_) {
      // Try to replace a page
      LRUReplacementPage *unpinnedPage = nullptr;
      for (auto page : lruList_) {
        if (!page->pinned) {
          unpinnedPage = page;
          break;
        }
      }
      if (unpinnedPage == nullptr) {
        // All pages are pinned
        return nullptr;
      }
      // Evict the unpinned page
      LRUReplacementPage *evicted = unpinnedPage;
      pages_.erase(evicted->pageId);
      lruList_.remove(evicted);
    }

    // Allocate new page and insert into cache and LRU list
    auto newPage = std::make_unique<LRUReplacementPage>(pageSize_, extraSize_, pageId, false);
    LRUReplacementPage *result = newPage.get();
    pages_[pageId] = std::move(newPage);
    lruList_.push_front(result);
    result->pinned = true;
    return result;
  }
  // Page already in cache
  LRUReplacementPage *result = static_cast<LRUReplacementPage *>(found->second.get());
  ++numHits_; // Increment the numHits_ here

  if (!result->pinned) {
    // Update LRU list
    lruList_.remove(result);
    lruList_.push_front(result);
  }
  result->pinned = true;
  return result;
}
/**
   * Unpin a page. The page is unpinned regardless of the number of prior
   * fetches, meaning it can be safely discarded. If `discard` is true, discard
   * the page. If `discard` is false, examine the number of pages in the cache.
   * If the number of pages in the cache is greater than the maximum, discard
   * the page.
   * @param page Pointer to a page.
   * @param discard Discard the page.
   */
void LRUReplacementPageCache::unpinPage(Page *page, bool discard) {

  auto *lruPage = static_cast<LRUReplacementPage *>(page);

  if (discard) {
    pages_.erase(lruPage->pageId);
    lruList_.remove(lruPage);
  } else {
    lruPage->pinned = false;
    // Update the LRU list
    lruList_.remove(lruPage);
    lruList_.push_back(lruPage); // Move the unpinned page to the back of the list
  }
}

/**
   * Change the page ID associated with a page. If a page with page ID
   * `newPageId` is already in the cache, it is assumed that the page is
   * unpinned, and the page is discarded.
   * @param page Pointer to a page.
   * @param newPageId New page ID.
   */
void LRUReplacementPageCache::changePageId(Page *page, unsigned int newPageId) {
  auto *lruPage = (LRUReplacementPage *) page;
  // If the new page ID is the same as the old one, return early.
  if (lruPage->pageId == newPageId) {
    return;
  }
  // If a page with the new page ID already exists, assume it's unpinned and discard it.
  if (pages_.count(newPageId) > 0) {
    unpinPage(pages_[newPageId].get(), true);
  }

  // Update the page ID and the hash table entry.
  unsigned int oldPageId = lruPage->pageId;
  lruPage->pageId = newPageId;
  pages_[newPageId] = std::move(pages_[oldPageId]);
  pages_.erase(oldPageId);
}

  /**
   * Discard all pages with page IDs greater than or equal to `pageIdLimit`. If
   * any of these pages are pinned, then they are implicitly unpinned, meaning
   * they can be safely discarded.
   * @param pageIdLimit Page ID limit.
   */
void LRUReplacementPageCache::discardPages(unsigned int pageIdLimit) {
  // TODO: Implement.
  auto it = pages_.begin();
  while (it != pages_.end()) {
    if (it->first >= pageIdLimit) {
      // Check if the page is pinned
      if (it->second->pinned) {
        // If the page is pinned, unpin it
        it->second->pinned = false;
      }
      // Discard the page if it's not pinned
      if (!it->second->pinned) {
        lruList_.remove(it->second.get());
        it = pages_.erase(it);
      } else {
        ++it;
      }
    } else {
      ++it;
    }
  }
  //throw NotImplementedException("LRUReplacementPageCache::discardPages");
}
