#ifndef CS564_PROJECT_PAGE_CACHE_LRU_2_HPP
#define CS564_PROJECT_PAGE_CACHE_LRU_2_HPP

#include "page_cache.hpp"
#include <list>
#include <unordered_map>

class LRU2ReplacementPageCache : public PageCache {
public:
  LRU2ReplacementPageCache(int pageSize, int extraSize);

  void setMaxNumPages(int maxNumPages) override;

  [[nodiscard]] int getNumPages() const override;

  Page *fetchPage(unsigned int pageId, bool allocate) override;

  void unpinPage(Page *page, bool discard) override;

  void changePageId(Page *page, unsigned int newPageId) override;

  void discardPages(unsigned int pageIdLimit) override;

private:
  // TODO: Declare class members as needed.

    struct LRU2ReplacementPage : public Page {
    LRU2ReplacementPage(int pageSize, int extraSize, unsigned  pageId,
                          bool pinned);
    unsigned int pageId;
    bool pinned;
    bool dirty;
    int lruCounter1; // Counter for LRU list 1
    int lruCounter2; // Counter for LRU list 2
  };

  std::unordered_map<unsigned, std::unique_ptr<LRU2ReplacementPage>> pages_;
  //std::unordered_map<unsigned, LRUReplacementPage *> pages_;
  std::list<LRU2ReplacementPage *> lru2List_;
  std::list<LRU2ReplacementPage *> lru1List_;
  int maxNumPages_;
  int numFetches_;
  int numHits_;
};

#endif // CS564_PROJECT_PAGE_CACHE_LRU_2_HPP
