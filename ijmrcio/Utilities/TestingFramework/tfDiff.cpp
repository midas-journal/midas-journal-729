

#include <memory>
#include <vector>
#include <iostream>
#include <list>
#include <map>
#include <assert.h>
#include <climits>
#include <limits>

using std::string;
using namespace std;
namespace testutil {
  namespace {
    typedef std::size_t hash_type;    
    typedef size_t id_type;        // type used for unique identifier
      
    // used by midpoint
    struct partition {
      ptrdiff_t xmid, ymid; // midpoints of the partition
      unsigned int length;  // length of the entire segment; the
      // length to xmid,ymid is ceil(length/2) 
    };
      
    // represents an edit to a sequence
    //
    // delete [line0, line0 + deleted)
    // insert [line1, line1 + inserted) before line0
    struct edit {
      edit(const ptrdiff_t _line0, 
	   const ptrdiff_t _line1,  
	   const ptrdiff_t _inserted,  
	   const ptrdiff_t _deleted) 
	: line0(_line0), line1(_line1), inserted(_inserted), deleted(_deleted) {
	//cout << "lines: " << line0 << " " << line1 << " ops: " << inserted << " " << deleted << "\n";
      };
	
      ptrdiff_t line0;      // line number in 0
      ptrdiff_t line1;      // line number in 1
      ptrdiff_t inserted;   // number of items inserted
      ptrdiff_t deleted;    // number of items deleted
    };
      
      
    // indexing information for a line
    struct line {
      string::size_type lindex;    // the index of the first char of the line
      string::size_type length;    // the length of the line
    };
      
      
    struct fumoffu {      
      const string *str[2];  // the strings with the text in them
      size_t nlines[2];      // the number of lines in the strings
      line *lines[2];        // the indecies for the lines,
      // nlines[k] length
      id_type *vec[2];       // the uniques of each line, nlines[k]
      // length 
      ptrdiff_t *diagonals;  // used by midpoint
    };
      

    
    /// \brief rotates an unsigned values to the left by n
    ///
    ///
    /// does a circular rotation of the bits so that 
    /// 10100000 would be 01000001
    template<class T>
    inline T RotateLeft(const T value, std::size_t n) {
      //      STATIC_ASSERT(!std::numeric_limits<T>::is_signed);
      return value << n | (value) >> (sizeof(T)*CHAR_BIT - n);
    }
      
    /// given a has value and a new charactor returns a new hash value
    inline hash_type hash(hash_type oldHash, string::value_type c) {
      return c + RotateLeft(oldHash,7);
    }
      
    /// generates a hash value from a string
    hash_type hashString(const string &str) {
      hash_type h = 0;   
      for (string::const_iterator i = str.begin(); i != str.end();++i) 
	h = hash(h, *i);
      return h;
    }
      
    /// \brief Finds the midpoint of the shortest edit sequence.
    ///
    /// Uses the algorithm described in:
    /// "An O(ND) Difference Algorithm and its Variations", Eugene Myers,
    ///   Algorithmica Vol. 1 No. 2, 1986, pp. 251-266;
    ///
    void FindEditMidPoint(const ptrdiff_t xbegin, const ptrdiff_t xend, 
			  const ptrdiff_t ybegin, const ptrdiff_t yend,
			  struct partition &part, fumoffu &data) {
	
      ptrdiff_t const dmin = xbegin - yend;  // minimum valid diagonal
      ptrdiff_t const dmax = xend - ybegin;  // maximum valid diagonal
      ptrdiff_t const fmid = xbegin - ybegin;// center diagonal for
      // forward iteration 
      ptrdiff_t const bmid = xend - yend;    // center diagonal for
      // backwards iteration  
      ptrdiff_t fmin = fmid, fmax = fmid;    // current bounds of
      // the forward search
      ptrdiff_t bmin = bmid, bmax = bmid;    // current bounds of
      // the backwards search
      bool odd = (fmid - bmid) & 1;          // true if opposing
      // corners are on odd
      // diagonals 

      const id_type *xvec = data.vec[0];    //
      const id_type *yvec = data.vec[1];    //

      // off set the two arrays, so that negative x will work
      // fdiag[k] is the forward max x pos in diagonal k
      ptrdiff_t *fdiag = data.diagonals - dmin + 1; 
      // bdiag[k] is the backwars min x pos is diagonal k 
      ptrdiff_t *bdiag = data.diagonals  + (dmax - dmin + 3) - dmin + 1;

      
 
      fdiag[fmid] = xbegin;
      bdiag[bmid] = xend;
      
      // length it the length(depth) of the current search
      for (ptrdiff_t length = 0; ; ++length) {
	

	// foward search of edit space one iteration on valid diagonals 
	if (fmin >= dmin) fdiag[fmin-1] = xbegin - 1;
	if (fmax <= dmax) fdiag[fmax+1] = xbegin - 1;

	// k is the diagonal
	for (ptrdiff_t k = fmax; k >= fmin; k-=2) {
	  ptrdiff_t x, y;
	  const ptrdiff_t lo = fdiag[k-1], hi = fdiag[k+1];
	  
	  if (lo < hi) 
	    x = hi;
	  else 
	    x = lo + 1;
	  y = x - k;
	  while (x < xend && y < yend && xvec[x] == yvec[y])
	    ++x, ++y;
	  fdiag[k] = x;
	  
	  if (odd && bmin <= k && k <= bmax && bdiag[k] <= x) {
	    // x,y is mid point
	    part.xmid = x;
	    part.ymid = y;
	    part.length = 2*length - 1;
	    return;
	  }	  
	  
	}
	// increment, after the end need to make sure we still
	// alternate even and odd, but that must be done later
	if (fmin > dmin) --fmin;
	if (fmax < dmax) ++fmax;
	
	
	// backwards search of edit space one iteration on valid diagonals 
	if (bmin >= dmin) bdiag[bmin-1] = xend + 1;
	if (bmax <= dmax) bdiag[bmax+1] = xend + 1;
	for (ptrdiff_t k = bmax; k >= bmin; k-=2) {
	  ptrdiff_t x, y;	  
	  const ptrdiff_t lo = bdiag[k-1], hi = bdiag[k+1];
	  
	  if (lo >= bdiag[k+1]) 
	    x = hi - 1;
	  else 
	    x = lo;
	  y = x - k;
	  while (x > xbegin && y >  ybegin && xvec[x-1] == yvec[y-1])
	    --x, --y;
	  bdiag[k] = x;
	  if (!odd && fmin <= k && k <= fmax && x <= fdiag[k]) {
	    
	    // x,y is mid point;
	    part.xmid = x; 
	    part.ymid = y;
	    part.length = 2*length;
	    return;
	  }	  
	  
	}

	// after the end need to make sure we still
	// alternate even and odd, hear only for the 1x1 case
	if (fmin < dmin) fmin += 2;
	if (fmax > dmax) fmax -= 2;
	
	
	// increment, after the end need to make sure we still
	// alternate even and odd
	bmin > dmin ? --bmin : ++bmin;
	bmax < dmax ? ++bmax : --bmax;

	
      }
      
    }

    
    /// \brief generates the shortest edit list
    list<edit> GenerateEditList(const ptrdiff_t xbegin, const ptrdiff_t xend, 
				const ptrdiff_t ybegin, const ptrdiff_t yend, fumoffu &data) {
      //cout << "+xbegin: " << xbegin << " xend: " << xend << " ybegin: " << ybegin << " yend: " << yend << endl;
      
      const id_type *xvec = data.vec[0];    //
      const id_type *yvec = data.vec[1];    //

      list<edit> l;        // 

      // can't be size 0
      assert(xend - xbegin != 0 || yend - ybegin != 0); 
      
      // can't begin or end with a 'snake'
      assert(xvec[xbegin] != yvec[ybegin] || xend - xbegin == 0 || yend - ybegin == 0);
      assert(xvec[xend-1] != yvec[yend-1] || xend - xbegin == 0 || yend - ybegin == 0);

      
      if (xend - xbegin == 0) {
	// all we have is some inserts
	l.push_back(edit(xbegin, ybegin, yend-ybegin, 0));
      } else if (yend - ybegin == 0) {
	// all we have is some deletes
	l.push_back(edit(xbegin, ybegin, 0, xend-xbegin));
      } else if (xend - xbegin > 0 && yend - ybegin > 0) {

	partition part;
	
	// get the mid point of the best set of operatrions
	FindEditMidPoint(xbegin, xend, ybegin, yend, part, data);

	//cout << "mid: " << part.xmid << " " << part.ymid << " length: " <<  part.length << endl;
	if (part.length > 1) {
	  list<edit> l1, l2;

	  // eat what 'snake we can in the middle
	  // and get the operations for the two halves
	  if (xvec[part.xmid-1] == yvec[part.ymid-1] && part.xmid != xbegin && part.ymid != ybegin) {
	    l2 = GenerateEditList(part.xmid, xend, part.ymid, yend, data);
	    // eat it
	    while(part.xmid != xbegin && part.ymid != ybegin && xvec[part.xmid - 1] == yvec[part.ymid-1])
	      --part.xmid, --part.ymid;
	    l1 = GenerateEditList(xbegin, part.xmid, ybegin, part.ymid, data);
	  } else {	    
	    l1 = GenerateEditList(xbegin, part.xmid, ybegin, part.ymid, data);
	    // eat it
	    while (part.xmid != xend && part.ymid != yend && xvec[part.xmid] == yvec[part.ymid])
	      ++part.xmid, ++part.ymid;
	    l2 = GenerateEditList(part.xmid, xend, part.ymid, yend, data);
	  }
	  
	  // min lenght is 2, must be two nonzero halves
	  assert(!l1.empty() && !l2.empty()); 
	  
	  // if the edit are after each other, then combine them
	  if(l1.back().line0 + l1.back().deleted == l2.front().line0 &&
	     l1.back().line1 + l1.back().inserted == l2.front().line1) {
	    l1.back().inserted += l2.front().inserted;
	    l1.back().deleted += l2.front().deleted;
	    l2.pop_front();
	  }
	  

	  // merge the lists 
	  l.splice(l.end(), l1);	  
	  l.splice(l.end(), l2);
	  //cout << "list length: " << l.size() << endl;
	} else if (part.length == 0) {  
	  // should beable to get here
	  assert(part.length);
	} else if ( yend - ybegin > xend - xbegin) {
	  
	  // we have an insert before or after the snake
	  if (xvec[xbegin] == yvec[ybegin])     	
	    l.push_back(edit(xend, yend-1, 1, 0));
	    
	  else 
	    l.push_back(edit(xbegin, ybegin, 1, 0));
	  	  
	} else {

	  // have a delete before or after the snake
	  if (xvec[xbegin] == yvec[ybegin]) 	    
	    l.push_back(edit(xend-1, ybegin, 0, 1));
	  else 
	    l.push_back(edit(xbegin, ybegin, 0, 1));	 
	}
	
      }
      
      //cout << "-xbegin: " << xbegin << " xend: " << xend << " ybegin: " << ybegin << " yend: " << yend << endl;      
      return l;
    }

    void PrintLines(const string &prefix, int source,  size_t lnum, size_t num, const fumoffu &data, std::ostream &os) {
		for (; num > 0; ++lnum, --num) {
			// some reason MSDEV needed conversion??
	os << prefix.c_str() << data.str[source]->substr(data.lines[source][lnum].lindex, 
						   data.lines[source][lnum].length).c_str()<< endl;
		}
    }

    /// \brief prints an edit list
    void PrintEditList(const list<edit> &l, const fumoffu &data, std::ostream &os) {
         
      for(list<edit>::const_iterator i = l.begin(); i != l.end(); ++i) {
	// the output is one-based indexing, but we have been using 0
	// the output is [i,j], but we have been using [begin,end)
	
	if (i->inserted && i->deleted) {
	  // 'FcT'
	  // Replace the items in ramge F of the first sequence with
	  // intems in the range of T of the second sequence.
	  if (i->deleted > 1)
	    os << i->line0 + 1 << "," << i->line0 + i->deleted;
	  else 
	    os << i->line0 + 1;
	  os << "c";
	  if (i->inserted > 1)
	    os << i->line1 + 1  << "," << i->line1 + i->inserted  << endl;
	  else 
	    os << i->line1 + 1  << endl;
	    
	  PrintLines("< ", 0, i->line0, i->deleted, data, os);
	  os << "----\n";
	  PrintLines("> ", 1, i->line1, i->inserted, data, os);
	} else if (i->inserted) {
	  // 'LaR'
	  // Added the item in range R of the second sequence after
	  // item L of the first sequence
	  os << i->line0 << "a";
	  if (i->inserted > 1)
	    os << i->line1 + 1  << "," << i->line1 + i->inserted  << endl;
	  else 
	    os << i->line1 + 1  << endl;	  
	  PrintLines("> ", 1, i->line1, i->inserted, data, os);
	} else {
	  // 'RdL'
	  // Delete the items in rande R from the first sequence;
	  // item L is where they would have appeared in the second
	  // sequence.
	  if (i->deleted > 1)
	    os << i->line0 + 1 << "," << i->line0 + i->deleted;
	  else 
	    os << i->line0 + 1;
	  os << "d" << i->line1 + 1 << endl;
	  PrintLines("< ", 0, i->line0, i->deleted, data, os);
	}
      }
    }

  
    /// \brief segments the strings into lines and unique ids
    void BuildLines(fumoffu &data){
      const string *str[2] = {data.str[0],  data.str[1]};
      
      
      // count the lines
      for (int j = 0; j < 2; ++j) {
	data.nlines[j] = 1; // one for the extra at the end
	for (string::size_type i = 0; i < str[j]->length(); ++i) {
	  if ((*str[j])[i] == '\n') {
	    data.nlines[j]++;
	  }
	}
      }
      
      // allocate the lines
      for (int j = 0; j < 2; ++j)
	data.lines[j] = new line[data.nlines[j]];

      // allocate the unique ids
      for (int j = 0; j < 2; ++j)
	data.vec[j] = new id_type[data.nlines[j]];
      

      // build the line indexing      
      for (int j = 0; j < 2; ++j) {
	string::size_type lindex = 0;
	size_t lnum = 0;
	for (string::size_type i = 0; i < str[j]->length(); ++i) {
	  if ((*str[j])[i] == '\n') {
	    data.lines[j][lnum].lindex = lindex;
	    data.lines[j][lnum].length = i - lindex;
	    ++lnum;
	    lindex = i + 1;	    
	  }
	}	
	// extra at the end, with "" if \n or the last line
	data.lines[j][lnum].lindex =  lindex;
	data.lines[j][lnum].length = str[j]->length() - lindex;
      }
      
      
      // build hash table
      // slow part
      typedef multimap<hash_type, size_t> hash_table;       // size_t is index into data.lines[?]
      hash_table hash[2];
      for (int j = 0; j < 2; ++j) {
	for (size_t i = 0; i < data.nlines[j]; ++i) {
	  hash_type h = hashString(str[j]->substr(data.lines[j][i].lindex, data.lines[j][i].length));
	  //   cout << "hashed: \"" << str[j]->substr(data.lines[j][i].lindex, data.lines[j][i].length) 
	  //  		 << "\" to " << h << endl;
	  hash[j].insert(make_pair(h, i));
	}
      }

      
      
      // convert the hash to unique ids
	  id_type id = std::numeric_limits<id_type>::min() + 1;
      while (true) {
	
	size_t hindex;	
	
	if (!hash[0].empty()) {
	  if (!hash[1].empty())
	    hindex = hash[1].begin()->first < hash[0].begin()->first;
	  else
	    hindex = 0;
	} else if (!hash[1].empty()) 
	  hindex = 1;
	else 
	  break;	  
	
	hash_table::value_type v  = *(hash[hindex].begin());
	  
	for (size_t j = hindex; j < 2; ++j) {
	  hash_table::iterator i = hash[j].begin();
	  while(!hash[j].empty()  && i->first == v.first) {
	    if (!str[hindex]->substr(data.lines[hindex][v.second].lindex, data.lines[hindex][v.second].length).compare(
		str[j]->substr(data.lines[j][i->second].lindex, data.lines[j][i->second].length))) {	      
	      data.vec[j][i->second] = id;	
	      // 		cout << "hashed: " << i->first << " to " << id << endl;
	      hash_table::iterator temp = i++;
	      hash[j].erase(temp);
	    } else {
	      ++i;
	    }
	  }

	}
	++id;

	
      }
      
    }

  } // namespace unamed 

    
  void Diff(const string &str1, const string &str2, std::ostream &os) {

      
    fumoffu data;

      
    data.str[0] = &str1;
    data.str[1] = &str2;
      
    BuildLines(data);

    // allocate one buffer for the two arrays
    data.diagonals = new ptrdiff_t[(data.nlines[0] + data.nlines[1]) *2 + 6];

    ptrdiff_t xbegin = 0;
    ptrdiff_t xend = data.nlines[0];
    ptrdiff_t ybegin = 0;
    ptrdiff_t yend = data.nlines[1];

      

    //       cout << "xbegin: " << xbegin << " xend: " << xend << " ybegin: " << ybegin << " yend: " << yend << endl;
    // eat all matching items at the beginging and the end, this
    // does not correspond to any output operation
    while(xbegin != xend && ybegin != yend && data.vec[0][xbegin] == data.vec[1][ybegin])
      ++xbegin, ++ybegin;
    while(xbegin != xend && ybegin != yend && data.vec[0][xend-1] == data.vec[1][yend-1])
      --xend, --yend;   
      
    //       cout << "xbegin: " << xbegin << " xend: " << xend << " ybegin: " << ybegin << " yend: " << yend << endl;
    list<edit> l;
    // if there is something left
    if (xbegin != xend || ybegin != yend)
      l = GenerateEditList(xbegin, xend, ybegin, yend, data);
      
    //       cout << "lenght: " << l.size() << endl;
   
    PrintEditList(l, data, os);
      
    delete[] data.diagonals;

    // allocate the lines
    for (int j = 0; j < 2; ++j)
      delete[] data.lines[j];

    // allocate the unique ids
    for (int j = 0; j < 2; ++j)
      delete[] data.vec[j];
      
  }
   
    
    
}
