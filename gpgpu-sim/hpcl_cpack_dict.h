
#ifndef HPCL_CPACK_DICT_H_
#define HPCL_CPACK_DICT_H_

#include <vector>
#include <map>
#include "hpcl_comp_pl_data.h"
#include <cassert>
#include <cmath>
#include <utility>
#include <unordered_set>

using namespace std;

template<class K>
class hpcl_cpack_dict 
{
public:
  hpcl_cpack_dict(unsigned int dict_size);
  ~hpcl_cpack_dict() {};
  void clear();
 

//compression
private:
  unsigned int m_MAX_DICT_SIZE;			//default: 256
public:
  vector<pair<string,string> > word_dict_glb;
  void update_dict(unordered_set<string> word_dict_lcl);
  string get_binary(int index);

};
template<class K>
hpcl_cpack_dict<K>::hpcl_cpack_dict(unsigned int dict_size)
{
	 m_MAX_DICT_SIZE = dict_size;	
}
template<class K>
void hpcl_cpack_dict<K>::clear()
{
  for(int i = 0; i < word_dict_glb.size(); i++) {
    delete word_dict_glb[i];
  }
  word_dict_glb.clear();
}

template<class K>
void hpcl_cpack_dict<K>::update_dict(unordered_set<string> word_dict_lcl)
{
	string insert_index;
	auto it = word_dict_lcl.begin();
	for(;it!=word_dict_lcl.end();it++)
	{
		string p = *it;
		int index=word_dict_glb.size();
		if(index==m_MAX_DICT_SIZE)
		{
			string index_s = word_dict_glb.front().second;
			word_dict_glb.erase(word_dict_glb.begin());
			word_dict_glb.push_back(make_pair(p,index_s));
		}
		else
		{
			// NORMAL CASE	
			word_dict_glb.push_back(make_pair(p,get_binary(index)));
		}
	}	
}


template<class K>
string hpcl_cpack_dict<K>::get_binary(int index)
{
	string output;
	int max_size=log2(MAX_DICT_SIZE);
	int a=index;
	char temp;

	while(max_size>0)
	{
		temp=index%2+'0';	
	//	output=to_string(index%2)+output;
		output=temp+output;
		index=index/2;
		max_size--;
	}
	return output;
}


#endif /* hpcl_cpack_DICT_H_ */

