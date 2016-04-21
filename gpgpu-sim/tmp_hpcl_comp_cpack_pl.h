// file for cpack comp run
#ifndef __CPACK_PL__
#define __CPACK_PL__

#include<iostream>
#include<vector>

#include "hpcl_comp_cpack_pl_proc.h"
#include "hpcl_comp_pl_data.h"

using namespace std;

enum hpcl_dict_repl
{
	LRU_CPACK=0,
	LFU_CPACK,
};
template<class K>
class hpcl_comp_cpack_pl
{
	private:
		unsigned m_pipeline_no;
		unsigned m_id;
		vector<hpcl_comp_cpack_pl_proc<K>*> m_comp_pl_proc;
		vector<mem_fetch*> m_comp_buffer;
		hpcl_comp_pl_data* m_input;
  		hpcl_comp_pl_data* m_output;
		unsigned m_comp_buffer_size;
		unsigned m_comp_state;
		enum comp_state
		{
			COMP_IDLE=0,
			COMP_RUN,
		};
		unsigned cur_pipeline;
		hpcl_comp_pl_data* m_output_data;




	public:
		hpcl_comp_cpack_pl(unsigned pipeline_no, unsigned id);
		void create(unsigned comp_buffer_size,unsigned dict_size, enum hpcl_dict_repl policy);
		mem_fetch* top_compressed_mem_fetch();
		void pop_compressed_mem_fetch();
		bool has_comp_buffer_space();
		hpcl_comp_pl_data* get_input();
		void run(); 



};
template<class K>
hpcl_comp_pl_data* hpcl_comp_cpack_pl<K>::get_input() {

	// set comp state to run
	m_comp_state = COMP_RUN;
	cur_pipeline=0;
	return m_input;
}
template<class K>
bool hpcl_comp_cpack_pl<K>::has_comp_buffer_space() {

	cout<<m_comp_buffer.size()<<"\t"<<m_comp_buffer_size<<endl;
  if(m_comp_buffer.size() >= m_comp_buffer_size)
  {
	  return false;
  }
  else
  {
	 return true;
  }
}
template<class K>
void hpcl_comp_cpack_pl<K>::pop_compressed_mem_fetch() {
  m_comp_buffer.erase(m_comp_buffer.begin());
}
template<class K>
hpcl_comp_cpack_pl<K>::hpcl_comp_cpack_pl(unsigned pipeline_no, unsigned id)
{
	m_pipeline_no = pipeline_no;
	m_id = id;
	m_input = new hpcl_comp_pl_data;	
	// set comp state to idle
	m_comp_state = COMP_IDLE;
	m_output_data = new hpcl_comp_pl_data;
	m_comp_buffer_size = -1;


}
template<class K>
mem_fetch* hpcl_comp_cpack_pl<K>::top_compressed_mem_fetch() {
  if(m_comp_buffer.size() == 0)	
	  return NULL;
  else				
	  return m_comp_buffer[0];
}
template<class K>
void hpcl_comp_cpack_pl<K>::create(unsigned comp_buffer_size,unsigned dict_size, enum hpcl_dict_repl policy)
{
	for(unsigned i = 0; i < m_pipeline_no; i++) 
  	{
		m_comp_pl_proc.push_back(new hpcl_comp_cpack_pl_proc<K>(dict_size,(int)policy/* mid eliminated , can add */));
		m_comp_pl_proc[i]->set_pl_index(i);
  	}

	for(unsigned i = 0; i < (m_pipeline_no-1); i++) 
	{
		m_comp_pl_proc[i]->set_output(m_comp_pl_proc[i+1]->get_input());
  	}
  	for(unsigned i = 0; i < m_pipeline_no-1; i++)
	{
		m_comp_pl_proc[i]->set_pl_type(hpcl_comp_cpack_pl_proc<K>::DUMMY);
	}
  	m_comp_pl_proc[m_pipeline_no-1]->set_pl_type(hpcl_comp_cpack_pl_proc<K>::COMP);
  	m_comp_pl_proc[m_pipeline_no-1]->set_output(new hpcl_comp_pl_data, 1);
  	m_comp_pl_proc[m_pipeline_no-1]->set_input(new hpcl_comp_pl_data);

 	m_comp_buffer_size = comp_buffer_size;	
	/* Pipe line stages 
	 *  OUTPUT <--- 4 <---  3 <-- , ....2 [0(get_data_from_1] 1(empty) ]<--- , 1 [0(empty) , 1(data)]  <-- INPUT
	 */
}
template<class K>
void hpcl_comp_cpack_pl<K>::run() 
{ /*
	if(m_comp_state == COMP_IDLE)
	{
		// update dict
		cout<<" ABPD .... update dict \n";
	}
	else
	{
		// run comp
		for(int i = 0; i < m_pipeline_no-1; i++) 
		{
			if(m_comp_pl_proc[i+1]->get_pl_type()==hpcl_comp_cpack_pl_proc<K>::COMP)
			{
				mem_fetch *mf = m_input->get_mem_fetch();
				if(mf)
				{
					cout<<"Abpd .. got valid data .. \n";
					m_comp_pl_proc[i+1]->get_input()->set_mem_fetch(mf);
					m_comp_pl_proc[i+1]->run();
				}
      				
			}
		
			hpcl_comp_pl_data* tmp = m_comp_pl_proc[i+1]->get_output(1);
      			if(tmp)
			{
				m_output_data->copy(tmp);
				if(i==0)
				{
					m_comp_buffer.push_back(tmp->get_mem_fetch());
    					assert(m_comp_buffer.size() <= m_comp_buffer_size);

				}
				else
				{
					 m_comp_pl_proc[i]->set_output(tmp,1);

				}
			}
		}		
	}*/
		mem_fetch *mf = m_input->get_mem_fetch();
				if(mf)
				{
					cout<<"Abpd .. got valid data .. \n";
					m_comp_pl_proc[8]->get_input()->set_mem_fetch(mf);
					m_comp_pl_proc[8]->run();
				}
					hpcl_comp_pl_data* tmp = m_comp_pl_proc[8]->get_output(1);
      			if(tmp)
			{

					m_comp_buffer.push_back(tmp->get_mem_fetch());
    					assert(m_comp_buffer.size() <= m_comp_buffer_size);

			}
}
#endif
