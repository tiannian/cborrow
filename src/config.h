#ifndef CBORROW_CONFIG_H
#define CBORROW_CONFIG_H

#define CBORROW_NAMESPACE_NAME cborrow

#define USE_STD

#if defined(USE_STD)

#include <functional>
#include <new>
#include <utility>

namespace CBORROW_NAMESPACE_NAME {

using std::function;
using std::forward;

}

#elif defined(USE_EMBED_STD)

namespace CBORROW_NAMESPACE_NAME {

//using estd::function;
inline void* operator new(std::size_t, void* __p) { return __p; }

}

#endif


#define CBORROW_DEBUG

#ifdef CBORROW_DEBUG

#include <iostream>

#define CBORROW_DBG(x) std::cout << x << std::endl

#endif

#undef USE_STD
#undef USE_EMBED_STD

#endif

