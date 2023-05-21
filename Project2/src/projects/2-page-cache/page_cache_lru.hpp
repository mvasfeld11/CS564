#ifndef CS564_PROJECT_PAGE_CACHE_LRU_HPP
#define CS564_PROJECT_PAGE_CACHE_LRU_HPP

#include "page_cache.hpp"
#include <list>
#include <unordered_map>

class LRUReplacementPageCache : public PageCache {
public:
  LRUReplacementPageCache(int pageSize, int extraSize);

  void setMaxNumPages(int maxNumPages) override;

  [[nodiscard]] int getNumPages() const override;

  Page *fetchPage(unsigned int pageId, bool allocate) override;

  void unpinPage(Page *page, bool discard) override;

  void changePageId(Page *page, unsigned int newPageId) override;

  void discardPages(unsigned int pageIdLimit) override;

private:
  // TODO: Declare class members as needed.

    struct LRUReplacementPage : public Page {
    LRUReplacementPage(int pageSize, int extraSize, unsigned int pageId,
                          bool pinned);
    unsigned int pageId;
    bool pinned;
    bool dirty;
  };

  std::unordered_map<unsigned, std::unique_ptr<LRUReplacementPage>> pages_;
  //std::unordered_map<unsigned, LRUReplacementPage *> pages_;
  std::list<LRUReplacementPage *> lruList_;
  int maxNumPages_;
  int numFetches_;
  int numHits_;
};

#endif // CS564_PROJECT_PAGE_CACHE_LRU_HPP
