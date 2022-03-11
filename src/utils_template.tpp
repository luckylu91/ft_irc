#pragma once

#include <algorithm>

template <class T1, class T2, class BinaryPredicate>
struct IsSame {
  T1 const & a;

  IsSame(T1 const & a): a(a) {}
  bool operator()(T2 const & b) {
    return BinaryPredicate()(a, b);
  }
};

template <class Compare, class A, class Vector>
typename Vector::iterator find_in_vector(A const & a, Vector & vec) {
  IsSame<A, typename Vector::value_type, Compare> compare_to_a(a);
  return std::find_if(
    vec.begin(),
    vec.end(),
    compare_to_a
  );
}

template <class Compare, class A, class Vector>
typename Vector::const_iterator find_in_vector(A const & a, Vector const & vec) {
  IsSame<A, typename Vector::value_type, Compare> compare_to_a(a);
  return std::find_if(
    vec.begin(),
    vec.end(),
    compare_to_a
  );
}

template <class Compare, class A, class Vector>
bool is_in_vector(A const & a, Vector const & vec) {
  return find_in_vector<Compare, A, Vector>(a, vec) != vec.end();
}
