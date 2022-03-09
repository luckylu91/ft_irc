#pragma once

#include <algorithm>

// template <class T>
// struct IsSameAsPointed {
//   T const & a;

//   IsSameAsPointed(T const & a): a(a) {}
//   bool operator()(T const * b) {
//     return this->a == *b;
//   }
// };

template <class T1, class T2, class BinaryPredicate>
struct IsSame {
  T1 const & a;

  IsSame(T1 const & a): a(a) {}
  bool operator()(T2 const & b) {
    return BinaryPredicate()(a, b);
  }
};

// template <class T>
// struct RefEqConstPointer {
//   bool operator()(T const & a, T const * b) {
//     return a == *b;
//   }
// };

// template <class A>
// typename std::vector<A const *>::iterator find_in_pointer_vector(A const & a, std::vector<A const *> & pointer_vec) {
//   IsSame<A, A const *, RefEqConstPointer<A> > compare_to_a(a);
//   return std::find_if(
//     pointer_vec.begin(),
//     pointer_vec.end(),
//     compare_to_a
//   );
// }

// template <class A>
// typename std::vector<A const *>::const_iterator find_in_pointer_vector(A const & a, std::vector<A const *> const & pointer_vec) {
//   return static_cast< typename std::vector<A const *>::const_iterator >(
//     find_in_pointer_vector(a, const_cast< std::vector<A const *> & >(pointer_vec))
//   );
// }

// template <class A>
// bool is_in_pointer_vector(A const & a, std::vector<A const *> const & pointer_vec) {
//   return find_in_pointer_vector(a, pointer_vec) != pointer_vec.end();
// }

template <class A, class AVector>
bool is_in(A const & a, AVector const & vec) {
  return std::find(vec.begin(), vec.end(), a) != vec.end();
}

template <class Compare, class A, class Vector>
typename Vector::iterator find_in_vector(A const & a, Vector & pointer_vec) {
  IsSame<A, typename Vector::value_type, Compare> compare_to_a(a);
  return std::find_if(
    pointer_vec.begin(),
    pointer_vec.end(),
    compare_to_a
  );
}

template <class Compare, class A, class Vector>
bool found_in(A const & a, Vector const & vec) {
  return find_in_vector<Compare, A, Vector>(a, vec) != vec.end();
}
