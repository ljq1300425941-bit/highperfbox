#include"hp/core/app.hpp"
#include"hp/cache/lru_cache.hpp"

int main()
{
    hp::cache::LRUCache<int, int> cache(10);
    hp::core::App app;
    app.run();
    return 0;
}