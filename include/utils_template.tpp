#pragma once

#include <algorithm>

template <class T1, class T2, class BinaryPredicate, class ReturnType>
struct ParametrizedUnaryFunction {
  T1 & a;

  ParametrizedUnaryFunction(T1 & a): a(a) {}
  ReturnType operator()(T2 & b) {
    return BinaryPredicate()(a, b);
  }
};

template <class Compare, class A, class Vector>
typename Vector::iterator find_in_vector(A & a, Vector & vec) {
  ParametrizedUnaryFunction<A, typename Vector::value_type, Compare, bool> compare_to_a(a);
  return std::find_if(
    vec.begin(),
    vec.end(),
    compare_to_a
  );
}

template <class Compare, class A, class Vector>
typename Vector::const_iterator find_in_vector(A & a, Vector const & vec) {
  ParametrizedUnaryFunction<A, const typename Vector::value_type, Compare, bool> compare_to_a(a);
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

template <class Action, class A, class Vector>
void for_each_in_vector(A & a, Vector & vec) {
  ParametrizedUnaryFunction<A, typename Vector::value_type, Action, void> act_on_a(a);
  std::for_each(
    vec.begin(),
    vec.end(),
    act_on_a
  );

}

template <class A, class Vector>
void remove_from_vector(A const & a, Vector & vec) {
  typename Vector::iterator it = std::remove(vec.begin(), vec.end(), a);
  vec.resize(static_cast<size_t>(it - vec.begin()));
}
