/*

    Copyright 2016 Carl Lei

    This file is part of Bamboo Shoot 3.

    Bamboo Shoot 3 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Bamboo Shoot 3 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Bamboo Shoot 3.  If not, see <http://www.gnu.org/licenses/>.

    Carl Lei <xecycle@gmail.com>

*/

#include <cassert>
#include <algorithm>
#include <vector>
#include <sstream>
#include <bs3/utils/iter-util.hh>

struct ncpas {
  ncpas() = default;
  ncpas(const ncpas&) = default;
  ncpas& operator=(const ncpas&) = delete;
};

int main()
{

  using pbsu::transformed_writer;
  using pbsu::multiplex_writer;
  using pbsu::spread_writer;
  using pbsu::filtering_writer;

  {
    // transformed writer
    int a[3] {};
    auto w = transformed_writer([](int x) { return x+1; }, a);
    *w++=2, *w++=2, *w++=2;
    assert(a[0]==3 && a[1]==3 && a[2]==3);
  }

  {
    // multiplex writer
    int a[3] {};
    double b[34] {};
    auto w = multiplex_writer(a, b);
    *w++=1, *w++=2, *w++=3;
    assert(a[0]==1 && a[1]==2 && a[2]==3);
    assert(b[0]==1 && b[1]==2 && b[2]==3);
  }

  {
    // spread writer
    int a[1] {};
    double b[1] {};
    auto w = spread_writer(a, b);
    *w++ = std::make_tuple(1, 2.0);
    assert(*a==1 && *b==2);
  }

  {
    // filtering writer
    std::vector<int> out;
    auto w = filtering_writer([](int x) { return x>=0; }, std::back_inserter(out));
    *w++=0, *w++=-1, *w++=1, *w++=-2, *w++=2;
    assert((out == std::vector<int>{0, 1, 2}));
  }

  {
    // mapping
    {
      // default case, strictly in-order once-exactly calls mapper
      using pbsu::make_mapping_iterator;

      {
        // simple one
        int arr[] = { 1, 2 };
        auto square = [](int x) { return x*x; };
        auto begin = make_mapping_iterator(square, arr);
        auto end = make_mapping_iterator(square, arr+2);
        static_assert(
          std::is_same<decltype(begin)::iterator_category, std::input_iterator_tag>::value,
          "mapping_iterator can be only input iterator");
        assert(begin==begin);
        assert(!(begin==end));
        assert(begin!=end);
        assert(*begin==1);
        ++begin;
        assert(*begin==4);
        ++begin;
        assert(begin==end);
      }

      {
        // operator->
        auto f = [](int x) { return std::make_pair(0, x); };
        int a = 1;
        assert(make_mapping_iterator(f, &a)->second==1);
      }

      {
        // input iterator
        std::istringstream in("1 2 3");
        auto addone = [](int x) { return 1+x; };
        auto begin = make_mapping_iterator(addone, std::istream_iterator<int>(in));
        auto end = make_mapping_iterator(addone, std::istream_iterator<int>());
        assert(*begin==2), ++begin;
        assert(*begin==3), ++begin;
        assert(*begin==4), ++begin;
        assert(begin==end);
      }

      {
        // multiple dereference
        struct nonpure { int a; int operator()(int) { return ++a; } };
        int src[1];
        auto iter = make_mapping_iterator(nonpure{0}, src);
        auto first = *iter;
        auto second = *iter;
        auto third = *iter;
        assert(first == second);
        assert(second == third);
      }

      {
        // does not skip calling on unused values
        struct counter {
          int i=0;
          int operator()(int) { return ++i; }
        };

        int arr[] = { 0, 0, 0 };
        auto it = make_mapping_iterator(counter(), arr);
        ++it, ++it;
        assert(*it==3);
      }

      {
        // allow non-copy-assignable
        int arr[] { 0 };
        auto it = make_mapping_iterator([](int) -> ncpas { return {}; }, arr);
        (void)*it;
      }

      {
        // select overload by source_iterator::reference
        struct check {
          int operator()(int&) { return 0; }
          int operator()(const int&) = delete;
          int operator()(int&&) = delete;
          int operator()(const int&&) = delete;
        };
        int arr[] { 0 };
        auto it = make_mapping_iterator(check(), arr);
        (void)*it;
      }
    }

    {
      // allows skipping over unused values
      using pbsu::make_skipping_mapping_iterator;

      {
        // simple one
        int arr[] = { 1, 2 };
        auto square = [](int x) { return x*x; };
        auto begin = make_skipping_mapping_iterator(square, arr);
        auto end = make_skipping_mapping_iterator(square, arr+2);
        static_assert(
          std::is_same<decltype(begin)::iterator_category, std::input_iterator_tag>::value,
          "mapping_iterator can be only input iterator");
        assert(begin==begin);
        assert(!(begin==end));
        assert(begin!=end);
        assert(*begin==1);
        ++begin;
        assert(*begin==4);
        ++begin;
        assert(begin==end);
      }

      {
        // operator->
        auto f = [](int x) { return std::make_pair(0, x); };
        int a = 1;
        assert(make_skipping_mapping_iterator(f, &a)->second==1);
      }

      {
        // input iterator
        std::istringstream in("1 2 3");
        auto addone = [](int x) { return 1+x; };
        auto begin = make_skipping_mapping_iterator(addone, std::istream_iterator<int>(in));
        auto end = make_skipping_mapping_iterator(addone, std::istream_iterator<int>());
        assert(*begin==2), ++begin;
        assert(*begin==3), ++begin;
        assert(*begin==4), ++begin;
        assert(begin==end);
      }

      {
        // multiple dereference
        struct nonpure { int a; int operator()(int) { return ++a; } };
        int src[1];
        auto iter = make_skipping_mapping_iterator(nonpure{0}, src);
        auto first = *iter;
        auto second = *iter;
        auto third = *iter;
        assert(first == second);
        assert(second == third);
      }

      {
        // does skip over calling on unused values
        struct counter {
          int i=0;
          int operator()(int) { return ++i; }
        };

        int arr[] = { 0, 0, 0 };
        auto it = make_skipping_mapping_iterator(counter(), arr);
        ++it, ++it;
        assert(*it==1);
      }

      {
        // allow non-copy-assignable
        int arr[] { 0 };
        auto it = make_skipping_mapping_iterator([](int) -> ncpas { return {}; }, arr);
        (void)*it;
      }

      {
        // select overload by source_iterator::reference
        struct check {
          int operator()(int&) { return 0; }
          int operator()(const int&) = delete;
          int operator()(int&&) = delete;
          int operator()(const int&&) = delete;
        };
        int arr[] { 0 };
        auto it = make_skipping_mapping_iterator(check(), arr);
        (void)*it;
      }
    }

    {
      // ephemeral
      using pbsu::make_ephemeral_mapping_iterator;

      {
        // simple one
        int arr[] = { 1, 2 };
        auto square = [](int x) { return x*x; };
        auto begin = make_ephemeral_mapping_iterator(square, arr);
        auto end = make_ephemeral_mapping_iterator(square, arr+2);
        assert(begin<end);
        assert(begin<=end);
        assert(begin<=begin);
        assert(end>begin);
        assert(end>=begin);
        assert(end>=end);
        assert(begin==begin);
        assert(!(begin==end));
        assert(begin!=end);
        assert(end-begin == 2);
        assert(*begin==1 && begin[1]==4);
        assert(*begin++==1);
        assert(*begin++==4);
        assert(*(end-1)==4);
        assert(*--end==4);
        assert(*end--==4);
        assert(*end==1);
      }

      {
        // operator->
        auto f = [](int x) { return std::make_pair(0, x); };
        int a = 1;
        assert(make_ephemeral_mapping_iterator(f, &a)->second==1);
      }

      {
        // input iterator
        std::istringstream in("1 2 3");
        auto addone = [](int x) { return 1+x; };
        auto begin = make_ephemeral_mapping_iterator(addone, std::istream_iterator<int>(in));
        auto end = make_ephemeral_mapping_iterator(addone, std::istream_iterator<int>());
        assert(*begin==2), ++begin;
        assert(*begin==3), ++begin;
        assert(*begin==4), ++begin;
        assert(begin==end);
      }

      {
        // allow non-copy-assignable
        struct ncpas { ncpas& operator=(const ncpas&) = delete; };
        int arr[] { 0 };
        auto it = make_ephemeral_mapping_iterator([](int) -> ncpas { return {}; }, arr);
        (void)*it;
      }

      {
        // select overload by source_iterator::reference
        struct check {
          int operator()(int&) { return 0; }
          int operator()(const int&) = delete;
          int operator()(int&&) = delete;
          int operator()(const int&&) = delete;
        };
        int arr[] { 0 };
        auto it = make_ephemeral_mapping_iterator(check(), arr);
        (void)*it;
      }
    }

  }

  {
    // filtering
    using pbsu::make_filtering_iterator;

    auto odd = [](int x) -> bool { return x&1; };

    {
      // simple
      int a[] = { 1, 2, 3, 4, 5 };
      assert((std::vector<int>(make_filtering_iterator(odd, a, a+5),
                               make_filtering_iterator(odd, a+5, a+5))
              == std::vector<int> { 1, 3, 5 }));
    }

    {
      // at most forward_iterator
      auto it = make_filtering_iterator(odd, (int*)0, (int*)0);
      static_assert(std::is_same<
                      typename std::iterator_traits<decltype(it)>::iterator_category,
                      std::forward_iterator_tag
                    >::value,
                    "it should be at most a forward iterator");

      std::istringstream in("1");
      auto from_input = make_filtering_iterator(odd,
                                                std::istream_iterator<int>(in),
                                                std::istream_iterator<int>());
      static_assert(std::is_same<
                      typename std::iterator_traits<decltype(from_input)>::iterator_category,
                      std::input_iterator_tag
                    >::value,
                    "from_input should be input iterator");
    }

    {
      // always point to a good point or end
      int a[] = { 2, 4, 5 };
      auto x = make_filtering_iterator(odd, a, a+3);
      auto y = make_filtering_iterator(odd, a+1, a+3);
      assert(x==y);

      int b[] = { 2, 4, 6 };
      auto z = make_filtering_iterator(odd, b, b+3);
      auto zend = make_filtering_iterator(odd, b+3, b+3);
      assert(z==zend);
    }

    {
      // operator-> and post increment
      struct foo { int a; } arr[1] {{0}};
      auto it = make_filtering_iterator([](foo){ return true; }, arr, arr+1);
      assert(it++->a == 0);
    }

  }

  {
    // integral
    using iter = pbsu::integral_iterator<size_t>;
    iter x(3);
    assert(*x==3);
    ++x;
    assert(*x==4);
    --x, --x;
    assert(*x==2);
    x+=5;
    assert(x==iter(7));
    x-=5;
    assert(x==iter(2));
    assert(x+1 == iter(3));
    assert(x+5 == iter(7));
    assert(x+(-2) == iter(0));
    assert(x-1 == iter(1));
    assert(x==iter(2));
    assert(x!=iter(3));
    iter y(4);
    assert(x<y);
    assert(x<=y);
    assert(!(x>y));
    assert(!(x>=y));
    assert(x[4]==6);
    assert((*x++ == 2) && (*x == 3));
    assert((*x-- == 3) && (*x == 2));
    assert(y-x==2);
    assert(x-y==-2);

    iter default_construct;
    assert(*default_construct == 0);
  }

  return 0;
}
