#ifndef __itkGenericUtilities_h
#define __itkGenericUtilities_h

namespace itk 
{
namespace Local 
{


///////////////////////////////////////////////////
	
	
/// \brief An iterator adaptor to perform an operation on dereferencing
template <typename DereferenceType, typename Iterator >
class dereference_iterator 
  : public std::iterator<typename std::iterator_traits<Iterator>::iterator_category,
                         DereferenceType,
                         typename std::iterator_traits<Iterator>::difference_type>
{
private:
  Iterator iter;
	      
  typedef std::iterator<std::input_iterator_tag,
                        DereferenceType,
                        typename std::iterator_traits<Iterator>::difference_type> IteratorTraits;
	
public:
	      
  typedef typename IteratorTraits::value_type value_type;
  typedef typename IteratorTraits::difference_type difference_type;
  typedef typename IteratorTraits::pointer pointer;
  typedef typename IteratorTraits::reference reference;
	
	
  dereference_iterator(void);
  explicit dereference_iterator(Iterator i) : iter(i) {}
	      
  Iterator base(void) const {return this->iter;}
	
  reference operator*(void) const {return *(*(this->iter));}
  pointer operator->(void) const {
    return &this->operator*();
  }
	
  dereference_iterator &operator++(void) {++(this->iter); return *this;}
  dereference_iterator operator++(int) {dereference_iterator temp = *this; ++(*this); return temp;}
	
  dereference_iterator &operator--(void) {--(this->iter); return *this;}
  dereference_iterator operator--(int) {dereference_iterator temp = *this; --(*this); return temp;}
	
  value_type operator[](difference_type n) const {return *(*this + n);}
	      
  dereference_iterator &operator+=( difference_type n) {this->iter += n; return *this; }
  dereference_iterator operator+( difference_type n) const {dereference_iterator temp = *this; return temp += n;}
	
	      
  dereference_iterator &operator-=( difference_type n) {return (*this) += (-n);}
  dereference_iterator operator-( difference_type n) const {return (*this) + (-n);}
	
  difference_type operator-(const dereference_iterator& rhs) const {return this->iter - rhs.iter;}
	
  bool operator<(const dereference_iterator &rhs) const {return this->iter< rhs.iter;}
  bool operator>(const dereference_iterator &rhs) const {return rhs < *this;}
  bool operator<=(const dereference_iterator &rhs) const {return !(*this > rhs);} 
  bool operator>=(const dereference_iterator &rhs) const {return !(*this > rhs);}
	      
  bool operator==(const dereference_iterator& rhs) const { return this->iter == rhs.iter;}
  bool operator!=(const dereference_iterator& rhs) const {return !(*this == rhs);}      
};
	
	


///////////////////////////////////////////////////
// Generic Programming Algorithms
///////////////////////////////////////////////////

/// \brief returns the min and max of a sequence defined by two iterators
///
/// Uses the < operator to determin ordering
/// If first == last then return is pair(first,first);
/// otherwise is it pair(min, max)
template <class InputIter>
std::pair<InputIter, InputIter> min_max_element(InputIter first, InputIter last) 
{
  std::pair<InputIter,InputIter> result(first,first);
  
  if (first == last) 
    return result;
  
  while (++first != last) 
    {
    InputIter prev = first;
    if (++first == last) 
      {
      if (*prev < *(result.first))
        result.first = prev;
      if (*(result.second) < *prev)
        result.second = prev;
      break;
      }
    else if (*first < *prev) 
      {
      if (*first < *(result.first))
        result.first = first;
      if (*(result.second) < *prev)
        result.second = prev;
      } 
    else 
      {
      if (*prev < *(result.first))
        result.first = prev;
      if (*(result.second) < *first)
        result.second = first;
      }
    }
  return result;
}
	    
	 
/// \brief returns the min and max of a 
///
/// Uses the provided binary functor
/// If first == last then return is pair(first,first);
/// otherwise is it pair(min, max)
template <class InputIter, class Compare>
std::pair<InputIter, InputIter> min_max_element(InputIter first, InputIter last, Compare comp) 
{
  std::pair<InputIter,InputIter> result(first,first);
	      
  if (first == last) 
    return result;
  
  while (++first != last) 
    {
    InputIter prev = first;
    if (++first == last) 
      {
      if (comp(*prev,*(result.first)))
        result.first = prev;
      if (comp(*(result.second),*prev))
        result.second = prev;
      break;
      }
    else if (comp(*first,*prev)) 
      {
      if (comp(*first,*(result.first)))
        result.first = first;
      if (comp(*(result.second),*prev))
        result.second = prev;
      } 
    else 
      {
      if (comp(*prev,*(result.first)))
        result.first = prev;
      if (comp(*(result.second),*first))
        result.second = first;
      }
    }
  return result;
}

} // end local namespace
} // end itk namespace

#endif //__itkGenericUtilities_h
