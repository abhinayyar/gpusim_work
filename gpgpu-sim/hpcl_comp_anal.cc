/*
 * hpcl_comp_anal.cc
 *
 *  Created on: Mar 8, 2016
 *      Author: mumichang
 */

#include "hpcl_comp_anal.h"
#include <cassert>

hpcl_comp_anal::hpcl_comp_anal ()
{
  // TODO Auto-generated constructor stub

}

hpcl_comp_anal::~hpcl_comp_anal ()
{
  // TODO Auto-generated destructor stub
}


void hpcl_comp_anal::create()
{
  m_comp_table_hit_no = new hpcl_stat;
  m_comp_table_miss_no = new hpcl_stat;
  m_ovr_avg_comp_table_hit_rate = new hpcl_stat;
}


void hpcl_comp_anal::add_sample(enum sample_type type, double val)
{
  if(type == GLOBAL_TABLE_HIT_NO)  	 m_comp_table_hit_no->add_sample(val);
  else if(type == GLOBAL_TABLE_MISS_NO)  m_comp_table_miss_no->add_sample(val);
  else if(type == READ_REPLY_COMP_RATIO) m_read_reply_comp_ratio->add_sample(val);
  else	assert(0);
}

void hpcl_comp_anal::display(std::ostream & os) const
{
  os << "====== hpcl_comp_anal ======" << std::endl;
  os << "gpu_tot_ovr_avg_comp_table_hit_rate = " << m_ovr_avg_comp_table_hit_rate->avg() << std::endl;
  os << "gpu_tot_ovr_avg_read_reply_comp_ratio = " << m_ovr_avg_read_reply_comp_ratio->avg() << std::endl;
  os << "===============================" << std::endl;
}

void hpcl_comp_anal::update_overall_stat()
{
  m_ovr_avg_comp_table_hit_rate->add_sample(m_comp_table_hit_no->sum()/(m_comp_table_hit_no->sum()+m_comp_table_miss_no->sum())*100);
  m_ovr_avg_read_reply_comp_ratio->add_sample(m_read_reply_comp_ratio->avg());
}

void hpcl_comp_anal::clear()
{
  m_comp_table_hit_no->clear();
  m_comp_table_miss_no->clear();
  m_read_reply_comp_ratio->clear();
}
