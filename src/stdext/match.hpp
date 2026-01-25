#ifndef __jetblack__stdext__match_hpp
#define __jetblack__stdext__match_hpp

// For rust style matching with variants.

namespace stdext
{
  template<class... Ts> struct match : Ts... { using Ts::operator()...; };
  template<class... Ts> match(Ts...) -> match<Ts...>;
}

#endif // __jetblack__stdext__match_hpp
