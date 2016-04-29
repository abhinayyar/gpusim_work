// Copyright (c) 2009-2011, Tor M. Aamodt
// The University of British Columbia
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// Redistributions in binary form must reproduce the above copyright notice, this
// list of conditions and the following disclaimer in the documentation and/or
// other materials provided with the distribution.
// Neither the name of The University of British Columbia nor the names of its
// contributors may be used to endorse or promote products derived from this
// software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef MEM_FETCH_H
#define MEM_FETCH_H

#include "addrdec.h"
#include "../abstract_hardware_model.h"
#include <bitset>

enum mf_type {
   READ_REQUEST = 0,
   WRITE_REQUEST,
   READ_REPLY, // send to shader
   WRITE_ACK,
   CTRL_MSG
};

#define MF_TUP_BEGIN(X) enum X {
#define MF_TUP(X) X
#define MF_TUP_END(X) };
#include "mem_fetch_status.tup"
#undef MF_TUP_BEGIN
#undef MF_TUP
#undef MF_TUP_END

class mem_fetch {
public:
    mem_fetch( const mem_access_t &access, 
               const warp_inst_t *inst,
               unsigned ctrl_size, 
               unsigned wid,
               unsigned sid, 
               unsigned tpc, 
    	       const class memory_config *config );
    
    // added by abpd
    mem_fetch(enum mf_type type,unsigned tpc,unsigned size,
		    unsigned wid,
               unsigned sid)
    {
	    m_type = type;
	    m_tpc = tpc;
	    m_sid= sid;
	    m_wid=wid;
	    //m_ctrl_size = size;
	    	m_status_timestamp.resize(END_STATUS+1,0);

    }
    
   ~mem_fetch();

   void set_status( enum mem_fetch_status status, unsigned long long cycle );
   void set_reply() 
   { 
       assert( m_access.get_type() != L1_WRBK_ACC && m_access.get_type() != L2_WRBK_ACC );
       if( m_type==READ_REQUEST ) {
           assert( !get_is_write() );
           m_type = READ_REPLY;
       } else if( m_type == WRITE_REQUEST ) {
           assert( get_is_write() );
           m_type = WRITE_ACK;
       }
   }
   void do_atomic();

   void print( FILE *fp, bool print_inst = true ) const;
   mem_access_t  get_access(){
		   return m_access;}
   const addrdec_t &get_tlx_addr() const { return m_raw_addr; }
   unsigned get_data_size() const { return m_data_size; }
   void     set_data_size( unsigned size ) { m_data_size=size; }
   unsigned get_ctrl_size() const { return m_ctrl_size; }
   unsigned size() const { return m_data_size+m_ctrl_size; }
   bool is_write() {return m_access.is_write();}
   void set_addr(new_addr_type addr) { m_access.set_addr(addr); }
   new_addr_type get_addr() const { return m_access.get_addr(); }
   new_addr_type get_partition_addr() const { return m_partition_addr; }
   unsigned get_sub_partition_id() const { return m_raw_addr.sub_partition; }
   bool     get_is_write() const { return m_access.is_write(); }
   unsigned get_request_uid() const { return m_request_uid; }
   unsigned get_sid() const { return m_sid; }
   unsigned get_tpc() const { return m_tpc; }
   unsigned get_wid() const { return m_wid; }
   bool istexture() const;
   bool isconst() const;
   enum mf_type get_type() const { return m_type; }
   bool isatomic() const;

   void set_return_timestamp( unsigned t ) { m_timestamp2=t; }
   void set_icnt_receive_time( unsigned t ) { m_icnt_receive_time=t; }
   unsigned get_timestamp() const { return m_timestamp; }
   unsigned get_return_timestamp() const { return m_timestamp2; }
   unsigned get_icnt_receive_time() const { return m_icnt_receive_time; }

   enum mem_access_type get_access_type() const { return m_access.get_type(); }
   const active_mask_t& get_access_warp_mask() const { return m_access.get_warp_mask(); }
   mem_access_byte_mask_t get_access_byte_mask() const { return m_access.get_byte_mask(); }

   address_type get_pc() const { return m_inst.empty()?-1:m_inst.pc; }
   const warp_inst_t &get_inst() { return m_inst; }
   enum mem_fetch_status get_status() const { return m_status; }

   const memory_config *get_mem_config(){return m_mem_config;}

   unsigned get_num_flits(bool simt_to_mem);
private:
   // request source information
   unsigned m_request_uid;
   unsigned m_sid;
   unsigned m_tpc;
   unsigned m_wid;

   // where is this request now?
   enum mem_fetch_status m_status;
   unsigned long long m_status_change;

   // request type, address, size, mask
   mem_access_t m_access;
   unsigned m_data_size; // how much data is being written
   unsigned m_ctrl_size; // how big would all this meta data be in hardware (does not necessarily match actual size of mem_fetch)
   new_addr_type m_partition_addr; // linear physical address *within* dram partition (partition bank select bits squeezed out)
   addrdec_t m_raw_addr; // raw physical address (i.e., decoded DRAM chip-row-bank-column address)
   enum mf_type m_type;

   // statistics
   unsigned m_timestamp;  // set to gpu_sim_cycle+gpu_tot_sim_cycle at struct creation
   unsigned m_timestamp2; // set to gpu_sim_cycle+gpu_tot_sim_cycle when pushed onto icnt to shader; only used for reads
   unsigned m_icnt_receive_time; // set to gpu_sim_cycle + interconnect_latency when fixed icnt latency mode is enabled

   // requesting instruction (put last so mem_fetch prints nicer in gdb)
   warp_inst_t m_inst;

   static unsigned sm_next_mf_request_uid;

   const class memory_config *m_mem_config;
   unsigned icnt_flit_size;

public:
   std::vector<unsigned long long> m_time_stamp;
   unsigned m_MC_id;
   std::vector<unsigned long long> m_data_size_hist;

   new_addr_type m_blk_addr;

//added by kh(070715)
   public:
   std::vector<unsigned long long> m_status_timestamp;
   ///

  //added by kh(070715)
public:
  void set_timestamp(enum mem_fetch_status status, unsigned long long cycle);
  unsigned long long get_timestamp(enum mem_fetch_status status);
  // added by abpd
  std::string compare_data;
  ///


  //added by kh(030816)
private:
  std::vector<unsigned char> m_real_data;
  // added by abpd
  std::vector<unsigned char> m_compress_data;

public:
  unsigned char* config_real_data(unsigned data_size);
  unsigned char get_real_data(unsigned data_index);
  void set_real_data(unsigned data_index, unsigned char data);
  void get_real_data_stream(unsigned flit_seq, std::vector<unsigned char>& dest, int stream_size=32);
  unsigned get_real_data_size();
  std::string get_comp_data(unsigned data_index);

  ///

  //added by kh(031716)
private:
  unsigned m_comp_data_size;

public:
  std::vector<unsigned char>& get_real_data_ptr();
  // added by abpd
  void set_comp_data(unsigned data_index, unsigned char data);
  void init_size() 
  { 
	  m_compress_data.resize(m_comp_data_size);  
  }
  void set_comp_data_size(unsigned comp_data_size) 	{	m_comp_data_size = comp_data_size;	}
  unsigned get_comp_data_size()		 		{	return m_comp_data_size;		}
  //unsigned size_after_comp() const 			{ 	return m_comp_data_size+m_ctrl_size; 	}
  unsigned get_comp_data_size_abpd();
  void get_compress_data_stream(unsigned flit_seq, std::vector<unsigned char>& dest, int stream_size=32);
};


#endif
