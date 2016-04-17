/*
 * hpcl_comp_config.h
 *
 *  Created on: Jan 4, 2016
 *      Author: mumichang
 */

#ifndef HPCL_COMP_CONFIG_H_
#define HPCL_COMP_CONFIG_H_

struct hpcl_comp_config {

  hpcl_comp_config()
  {
    hpcl_comp_en = 0;
    hpcl_comp_algo = GLOBAL_PRIVATE;
    hpcl_comp_buffer_size = 1;
    hpcl_comp_word_size = 2;
  }

  void init()
  {
  }

  void reg_options(class OptionParser * opp)
  {
    option_parser_register(opp, "-hpcl_comp_en", OPT_UINT32, &hpcl_comp_en,
			     "0: disable compression, 1: enable compression",
			     "0");

    option_parser_register(opp, "-hpcl_comp_algo", OPT_UINT32, &hpcl_comp_algo,
			   "0: global_table(private), 1: ",
			   "0");

    option_parser_register(opp, "-hpcl_comp_buffer_size", OPT_UINT32, &hpcl_comp_buffer_size,
  			   "1: default, ",
  			   "1");

    option_parser_register(opp, "-hpcl_comp_word_size", OPT_UINT32, &hpcl_comp_word_size,
  			   "2: default, ",
  			   "2");
  }


  enum comp_algo_type {
    GLOBAL_PRIVATE = 0,
    LOCAL_WORD_MATCHING=1,
    BDI_WORD_MATCHING=2,
    ABPD_LOCAL_WORD_MATCHING=3,
  };


  unsigned hpcl_comp_en;
  enum comp_algo_type hpcl_comp_algo;

  //added by kh(031916)
  unsigned hpcl_comp_buffer_size;
  unsigned hpcl_comp_word_size;


};




#endif /* HPCL_DIRECT_LINK_CONFIG_H_ */
