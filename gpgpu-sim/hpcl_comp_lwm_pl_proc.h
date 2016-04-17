/*
 * hpcl_comp_pl_proc.h
 *
 *  Created on: Feb 22, 2016
 *      Author: mumichang
 */

#ifndef HPCL_COMP_LWM_PL_PROC_H_
#define HPCL_COMP_LWM_PL_PROC_H_

#include "hpcl_comp_pl_data.h"
#include "hpcl_dict.h"
#include "hpcl_comp_data.h"
#include <vector>
#include <cassert>
#include <iostream>
#include <cmath>


//#define DUMMY_COMP_TEST 0

//#define DEBUG_LWM 1

//added by kh(030816)
#include "hpcl_comp_anal.h"
extern hpcl_comp_anal* g_hpcl_comp_anal;
///

template<class K>
class hpcl_comp_lwm_pl_proc {
private:
  hpcl_comp_pl_data* m_input;
  hpcl_comp_pl_data* m_output;
  int m_pl_index;

public:
  hpcl_comp_lwm_pl_proc();
  ~hpcl_comp_lwm_pl_proc() {
    if(m_input)	 	delete m_input;
    if(m_output)	delete m_output;
    if(m_loc_dict)	delete m_loc_dict;
  };
  void set_pl_index(int pl_index);
  void set_output(hpcl_comp_pl_data* output);
  hpcl_comp_pl_data* get_output();
  hpcl_comp_pl_data* get_input();
  void reset_output();

private:
  hpcl_dict<K>* m_loc_dict;		//1B word
  void decompose_data(std::vector<unsigned char>& cache_data, std::vector<K>& word_list);
  unsigned simul_local_comp(hpcl_dict<K>& loc_dict, vector<unsigned char>& cache_data);
public:
  void run();
};

template <class K>
hpcl_comp_lwm_pl_proc<K>::hpcl_comp_lwm_pl_proc() {
  m_input = new hpcl_comp_pl_data;
  m_output = NULL;
  m_pl_index = -1;
  m_loc_dict = new hpcl_dict<K>(128, HPCL_LFU);
  m_loc_dict->clear();
}

template <class K>
void hpcl_comp_lwm_pl_proc<K>::set_pl_index(int pl_index) {
  m_pl_index = pl_index;
}

template <class K>
void hpcl_comp_lwm_pl_proc<K>::set_output(hpcl_comp_pl_data* output) {
  m_output = output;
}

template <class K>
hpcl_comp_pl_data* hpcl_comp_lwm_pl_proc<K>::get_output() {
  return m_output;
}

template <class K>
hpcl_comp_pl_data* hpcl_comp_lwm_pl_proc<K>::get_input() {
  return m_input;
}

template <class K>
void hpcl_comp_lwm_pl_proc<K>::reset_output() {
  m_output->clean();
}

template<class K>
void hpcl_comp_lwm_pl_proc<K>::run()
{
  #ifdef DUMMY_COMP_TEST
  m_output->set_mem_fetch(m_input->get_mem_fetch());
  #else

  mem_fetch* mf = m_input->get_mem_fetch();
  m_output->set_mem_fetch(mf);

  if(!mf) return;

  //If mf is not type for compression,
  if(mf->get_real_data_size() == 0)	return;

  std::vector<unsigned char>& cache_data = mf->get_real_data_ptr();
  std::vector<K> word_list;
  decompose_data(cache_data, word_list);

  //LWM simulation
  //encodeing format: encoding flag(1 bit), encoded word(index_size or word_size)
  unsigned comp_bit_size = 0;
  unsigned data_ptr_bits = (unsigned) ceil(log2(cache_data.size()/sizeof(K)));

  #ifdef DEBUG_LWM
  printf("org_data = 0x");
  for(unsigned i = 0; i < cache_data.size(); i++) {
    printf("%u", cache_data[i]);
  }
  printf("\n");
  #endif

  for(unsigned i = 0; i < word_list.size(); i++) {
      comp_bit_size++;		//flag(1 bit)
      K word = word_list[i];
      int word_index = m_loc_dict->search_word(word);
      if(word_index >= 0) {
	comp_bit_size += data_ptr_bits;

	#ifdef DEBUG_LWM
	std::cout << "\t";
	m_loc_dict->print_word(word);
	std::cout << " is found!" << std::endl;
	#endif

      } else {
	comp_bit_size += (sizeof(K)*8);
	m_loc_dict->update_dict(word, 0);

	#ifdef DEBUG_LWM
	std::cout << "\t";
	m_loc_dict->print_word(word);
	std::cout << " is not found!" << std::endl;
	#endif
      }
  }

  unsigned comp_byte_size = ceil((double)comp_bit_size/8);
  mf->set_comp_data_size(comp_byte_size);

  #ifdef DEBUG_LWM
  if(comp_byte_size < mf->get_real_data_size()) {
    printf("access_type %u org_data = 0x", mf->get_access_type());
    for(unsigned i = 0; i < cache_data.size(); i++) {
      printf("%u", cache_data[i]);
    }
    printf("\n");
    printf("org_data_size = %d, comp_data_size = %d\n", mf->get_real_data_size(), comp_byte_size);
    m_loc_dict->print();
  }
  #endif

  //delete a local dictionary
  m_loc_dict->clear();

  #endif
}

template <class K>
void hpcl_comp_lwm_pl_proc<K>::decompose_data(std::vector<unsigned char>& cache_data, std::vector<K>& word_list)
{
	cout<<" abpd  cache size"<<cache_data.size()<<"\n";
  for(unsigned i = 0; i < cache_data.size(); i=i+sizeof(K)) {
    K word_candi = 0;
    for(int j = sizeof(K)-1; j >= 0; j--) {
      K tmp = cache_data[i+j];
      tmp = (tmp << (8*j));
      word_candi += tmp;
    }
    word_list.push_back(word_candi);
//    printf("\tword -- ");
//    hpcl_dict_elem<K>::print_word_data(word_candi);
//    printf("\n");
  }
  assert(word_list.size() == cache_data.size()/sizeof(K));
}




#endif /* HPCL_COMP_PL_PROC_H_ */
