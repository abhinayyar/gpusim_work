// actual code for cpack
#ifndef __CPACK_PL_PROC__
#define __CPACK_PL_PROC__

#include<iostream>
#include<vector>
#include "hpcl_comp_pl_data.h"
#include "hpcl_comp_data.h"

using namespace std;

template<class K>
class hpcl_comp_cpack_pl_proc
{
	public:
		enum pipeline_type
		{
			NONE=-1,
			COMP=0,
			DUMMY,
		};
		hpcl_comp_cpack_pl_proc(unsigned dict_size, /* Typecase enum */int policy); 
		void set_pl_index(int pl_index);
  		void set_pl_type(enum pipeline_type type);
  		void set_output(hpcl_comp_pl_data* output, int index=0);
  		hpcl_comp_pl_data* get_output(int index=0);
  		hpcl_comp_pl_data* get_input();
		void set_input(hpcl_comp_pl_data* input);
		void run();
		int get_pl_type(); 

	private:
  		hpcl_comp_pl_data* m_input;
  		hpcl_comp_pl_data* m_output[2];
  		int m_pl_index;
  		unsigned m_par_id;
		enum pipeline_type m_type;

};
template <class K>
hpcl_comp_cpack_pl_proc<K>::hpcl_comp_cpack_pl_proc(unsigned dict_size, /* Typecase enum */int policy) 
{
  m_input = new hpcl_comp_pl_data;
  m_output[0] = NULL;
  m_output[1] = NULL;
  m_pl_index = -1;
  m_type=NONE;

  /* create dict */
}
template <class K>
void hpcl_comp_cpack_pl_proc<K>::set_pl_index(int pl_index) {
  	m_pl_index = pl_index;
}

template <class K>
void hpcl_comp_cpack_pl_proc<K>::set_pl_type(enum pipeline_type type) {
  m_type = type;
}
template <class K>
int hpcl_comp_cpack_pl_proc<K>::get_pl_type() {
  return (int)m_type;
}

template <class K>
void hpcl_comp_cpack_pl_proc<K>::set_output(hpcl_comp_pl_data* output, int index) {
  m_output[index] = output;
}

template <class K>
hpcl_comp_pl_data* hpcl_comp_cpack_pl_proc<K>::get_output(int index) {
  return m_output[index];
}

template <class K>
hpcl_comp_pl_data* hpcl_comp_cpack_pl_proc<K>::get_input() {
  return m_input;
}

template <class K>
void hpcl_comp_cpack_pl_proc<K>::set_input(hpcl_comp_pl_data* input) {
	m_input = input;
}
template<class K>
void hpcl_comp_cpack_pl_proc<K>::run()
{

	mem_fetch *mf = m_input->get_mem_fetch();
	if(mf)
	{
		cout<<"Got valid data in run \n";
	}
	else
	{
		//cout<<"Has bad  invalid data in run \n";
	}
	
  	m_output[1]->set_mem_fetch(mf);

	int actual_data_size = mf->get_real_data_size();
	if(actual_data_size == 0 ) return;

	mf->set_comp_data_size(actual_data_size);
}

#endif
