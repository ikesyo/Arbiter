#pragma once

#ifndef __cplusplus
#error "This file must be compiled as C++."
#endif

#include <iterator>
#include <tuple>

namespace Arbiter {

/**
 * The value type for an iterator which combines the values of two other
 * iterators into a tuple.
 */
template<typename LeftIt, typename RightIt>
using TupledIteratorValue = std::tuple<typename std::iterator_traits<LeftIt>::value_type, typename std::iterator_traits<RightIt>::value_type>;

/**
 * The pointer type for an iterator which combines the values of two other
 * iterators into a tuple.
 */
template<typename LeftIt, typename RightIt>
using TupledIteratorPointer = std::tuple<typename std::iterator_traits<LeftIt>::pointer, typename std::iterator_traits<RightIt>::pointer>;

/**
 * The reference type for an iterator which combines the values of two other
 * iterators into a tuple.
 */
template<typename LeftIt, typename RightIt>
using TupledIteratorReference = std::tuple<typename std::iterator_traits<LeftIt>::reference, typename std::iterator_traits<RightIt>::reference>;

/**
 * The base class for an iterator which combines the values of two other
 * iterators into a tuple.
 */
template<typename Category, typename LeftIt, typename RightIt, typename Distance = std::ptrdiff_t>
using TupledIterator = std::iterator<Category, TupledIteratorValue<LeftIt, RightIt>, Distance, TupledIteratorPointer<LeftIt, RightIt>, TupledIteratorReference<LeftIt, RightIt>>;

/**
 * An iterator which generates every possible combination of the values of two
 * other iterators.
 *
 * Both input types must refer to forward iterators.
 */
template<typename LeftIt, typename RightIt>
class PermutationIterator final : public TupledIterator<std::forward_iterator_tag, LeftIt, RightIt>
{
  public:
    /**
     * Creates an empty iterator. The iterator must not be dereferenced.
     */
    PermutationIterator ()
    {}

    /**
     * Creates an iterator which will create all possible combinations between
     * the given ranges.
     */
    PermutationIterator (LeftIt leftBegin, LeftIt leftEnd, RightIt rightBegin, RightIt rightEnd)
      : _leftCurrent(std::move(leftBegin))
      , _leftEnd(std::move(leftEnd))
      , _rightBegin(std::move(rightBegin))
      , _rightEnd(std::move(rightEnd))
    {
      _rightCurrent = _rightBegin;
    }

    PermutationIterator &operator++ ()
    {
      if (_rightCurrent == _rightEnd) {
        _rightCurrent = _rightBegin;

        assert(_leftCurrent != _leftEnd);
        ++_leftCurrent;
      } else {
        ++_rightCurrent;
      }

      return *this;
    }

    PermutationIterator operator++ (int)
    {
      auto iter = *this;
      ++(*this);
      return iter;
    }

    bool operator== (const PermutationIterator &other) const
    {
      return _leftCurrent == other._leftCurrent && _rightCurrent == other._rightCurrent;
    }

    bool operator!= (const PermutationIterator &other) const
    {
      return !(*this == other);
    }

    TupledIteratorReference<LeftIt, RightIt> operator*() const
    {
      return std::make_tuple(*_leftCurrent, *_rightCurrent);
    }

    /**
     * Returns whether the iterator is valid (i.e., dereferenceable).
     *
     * If this is false, it is an error to increment the iterator further.
     */
    operator bool () const
    {
      return _leftCurrent != _leftEnd && _rightCurrent != _rightEnd;
    }

  private:
    LeftIt _leftCurrent;
    LeftIt _leftEnd;

    RightIt _rightBegin;
    RightIt _rightCurrent;
    RightIt _rightEnd;
};

} // namespace Arbiter
