/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Authors: Evan Klinger <evan715@sirius.com>                           |
   |          Timothy Whitfield <timothy@ametro.net>                      |
   +----------------------------------------------------------------------+
 */

#include "php.h"

#if HAVE_MCK
#include "cybercash.h"
#include "mckcrypt.h"
#include "base64.h"

function_entry cybercash_functions[] = {
	PHP_FE(cybercash_encr, NULL)
	PHP_FE(cybercash_decr, NULL)
	PHP_FE(cybercash_base64_encode, NULL)
	PHP_FE(cybercash_base64_decode, NULL)
	{0}
  };

zend_module_entry cybercash_module_entry = {
	"CyberCash",
	cybercash_functions,
	NULL,NULL,
	NULL,NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES,
};

PHP_FUNCTION(cybercash_encr)
{
	pval **wmk, **sk, **inbuff;
	unsigned char *outbuff, *macbuff;
	unsigned int outAlloc, outLth;
	long errcode;
  
	if(ARG_COUNT(ht) != 3 || zend_get_parameters_ex(3,&wmk,&sk,&inbuff) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(wmk);
	convert_to_string_ex(sk);
	convert_to_string_ex(inbuff);

	outAlloc = (*inbuff)->value.str.len + 10;

  outbuff = (unsigned char *)emalloc(outAlloc);
  macbuff = (unsigned char *)emalloc(20);

  errcode = mck_encr((*wmk)->value.str.val,(*sk)->value.str.val,
			(*inbuff)->value.str.len+1,
                   	(*inbuff)->value.str.val,
                   	outAlloc,
                   	outbuff,
                   	&outLth,
                   	macbuff);

	array_init(return_value);

	add_assoc_long(return_value,"errcode",errcode);

	if(!errcode)
	{
		add_assoc_stringl(return_value,"outbuff",outbuff,outLth,0);
      		add_assoc_long(return_value,"outLth",outLth);
		add_assoc_stringl(return_value,"macbuff",macbuff,20,0);
    	}
    	else
    	{
      		efree(outbuff);
      		efree(macbuff);
    	}
}

PHP_FUNCTION(cybercash_decr)
{
  pval **wmk,**sk,**inbuff;
  unsigned char *outbuff, *macbuff;
  unsigned int outAlloc, outLth;
  long errcode;
  

  if(ARG_COUNT(ht) != 3 || zend_get_parameters_ex(3,&wmk,&sk,&inbuff) == FAILURE)
  {
    WRONG_PARAM_COUNT;
  }

  convert_to_string_ex(wmk);
  convert_to_string_ex(sk);
  convert_to_string_ex(inbuff);

  outAlloc=(*inbuff)->value.str.len;

  outbuff=(unsigned char *)emalloc(outAlloc);
  macbuff=(unsigned char *)emalloc(20);

  errcode=mck_decr((*wmk)->value.str.val,
                   (*sk)->value.str.val,
                   (*inbuff)->value.str.len,
                   (*inbuff)->value.str.val,
                   outAlloc,
                   outbuff,
                   &outLth,
                   macbuff);

	array_init(return_value);

	add_assoc_long(return_value,"errcode",errcode);

	if(!errcode) {
		add_assoc_stringl(return_value,"outbuff",outbuff,outLth,0);
		add_assoc_long(return_value,"outLth",outLth);
		add_assoc_stringl(return_value,"macbuff",macbuff,20,0);
	}
    	else
    	{
      		efree(outbuff);
      		efree(macbuff);
    	}
}

PHP_FUNCTION(cybercash_base64_encode)
{
  pval **inbuff;
  char *outbuff;
  long ret_length;

  if(ARG_COUNT(ht) != 1 ||
     zend_get_parameters_ex(1,&inbuff) == FAILURE)
  {
    WRONG_PARAM_COUNT;
  }

  convert_to_string_ex(inbuff);

  outbuff=(char *)emalloc(
    base64_enc_size((unsigned int)(*inbuff)->value.str.len));
  
  ret_length=base64_encode(outbuff,
     (*inbuff)->value.str.val,(*inbuff)->value.str.len);

  return_value->value.str.val=outbuff;
  return_value->value.str.len=ret_length;
  return_value->type=IS_STRING;
  
}

PHP_FUNCTION(cybercash_base64_decode)
{
  pval **inbuff;
  char *outbuff;
  long ret_length;

  if(ARG_COUNT(ht) != 1 ||
     zend_get_parameters_ex(1,&inbuff) == FAILURE)
  {
    WRONG_PARAM_COUNT;
  }

  convert_to_string_ex(inbuff);

  outbuff=(char *)emalloc(
    base64_dec_size((unsigned int)(*inbuff)->value.str.len));
  
  ret_length=base64_decode(outbuff,
     (*inbuff)->value.str.val,(*inbuff)->value.str.len);

  return_value->value.str.val=outbuff;
  return_value->value.str.len=ret_length;
  return_value->type=IS_STRING;
  
}
#endif
