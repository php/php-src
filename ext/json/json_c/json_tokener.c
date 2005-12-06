/*
 * $Id$
 *
 * Copyright Metaparadigm Pte. Ltd. 2004.
 * Michael Clark <michael@metaparadigm.com>
 *
 * Copyright (c) 2005 Omar Kilani <omar@rmilk.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public (LGPL)
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details: http://www.gnu.org/
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "bits.h"
#include "debug.h"
#include "printbuf.h"
#include "arraylist.h"
#include "json_object.h"
#include "ossupport.h"
#include "json_tokener.h"

static struct json_object* json_tokener_do_parse(struct json_tokener *this, enum json_tokener_error *err);

struct json_object* json_tokener_parse(char * s)
{
  struct json_tokener tok;
  struct json_object* obj;
  enum json_tokener_error err;

  tok.source = s;
  tok.pos = 0;
  tok.pb = printbuf_new();
  obj = json_tokener_do_parse(&tok, &err);
  printbuf_free(tok.pb);
  return obj;
}

static struct json_object* json_tokener_do_parse(struct json_tokener *this, enum json_tokener_error *err)
{
  enum json_tokener_state state, saved_state;
  struct json_object *current = NULL, *obj;
  char *obj_field_name = NULL;
  char quote_char;
  int deemed_double, start_offset;
  char c;

  *err = json_tokener_success;
  state = json_tokener_state_eatws;
  saved_state = json_tokener_state_start;

  do {
    c = this->source[this->pos];
    switch(state) {

    case json_tokener_state_eatws:
      if(isspace(c)) {
	this->pos++;
      } else if(c == '/') {
	state = json_tokener_state_comment_start;
	start_offset = this->pos++;
      } else {
	state = saved_state;
      }
      break;

    case json_tokener_state_start:
      switch(c) {
      case '{':
	state = json_tokener_state_eatws;
	saved_state = json_tokener_state_object;
	current = json_object_new_object();
	this->pos++;
	break;
      case '[':
	state = json_tokener_state_eatws;
	saved_state = json_tokener_state_array;
	current = json_object_new_array();
	this->pos++;
	break;
      case 'N':
      case 'n':
	state = json_tokener_state_null;
	start_offset = this->pos++;
	break;
      case '"':
      case '\'':
	quote_char = c;
	printbuf_reset(this->pb);
	state = json_tokener_state_string;
	start_offset = ++this->pos;
	break;
      case 'T':
      case 't':
      case 'F':
      case 'f':
	state = json_tokener_state_boolean;
	start_offset = this->pos++;
	break;
#if defined(__GNUC__)
	  case '0' ... '9':
#else
	  case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
#endif
      case '-':
	deemed_double = 0;
	state = json_tokener_state_number;
	start_offset = this->pos++;
	break;
      default:
	*err = json_tokener_error_parse_unexpected;
	goto out;
      }
      break;

    case json_tokener_state_finish:
      goto out;

    case json_tokener_state_null:
      if(strncasecmp("null", this->source + start_offset,
		     this->pos - start_offset)) {
        *err = json_tokener_error_parse_null;
        return NULL;
      }
      if(this->pos - start_offset == 4) {
	current = NULL;
	saved_state = json_tokener_state_finish;
	state = json_tokener_state_eatws;
      } else {
	this->pos++;
      }
      break;

    case json_tokener_state_comment_start:
      if(c == '*') {
	state = json_tokener_state_comment;
      } else if(c == '/') {
	state = json_tokener_state_comment_eol;
      } else {
	*err = json_tokener_error_parse_comment;
	goto out;
      }
      this->pos++;
      break;

    case json_tokener_state_comment:
      if(c == '*') state = json_tokener_state_comment_end;
      this->pos++;
      break;

    case json_tokener_state_comment_eol:
      if(c == '\n') {
	if(mc_get_debug()) {
	  char *tmp = strndup(this->source + start_offset,
			      this->pos - start_offset);
	  mc_debug("json_tokener_comment: %s\n", tmp);
	  free(tmp);
	}
	state = json_tokener_state_eatws;
      }
      this->pos++;
      break;

    case json_tokener_state_comment_end:
      if(c == '/') {
	if(mc_get_debug()) {
	  char *tmp = strndup(this->source + start_offset,
			      this->pos - start_offset + 1);
	  mc_debug("json_tokener_comment: %s\n", tmp);
	  free(tmp);
	}
	state = json_tokener_state_eatws;
      } else {
	state = json_tokener_state_comment;
      }
      this->pos++;
      break;

    case json_tokener_state_string:
      if(c == quote_char) {
	printbuf_memappend(this->pb, this->source + start_offset,
			   this->pos - start_offset);
	current = json_object_new_string(this->pb->buf);
	saved_state = json_tokener_state_finish;
	state = json_tokener_state_eatws;
      } else if(c == '\\') {
	saved_state = json_tokener_state_string;
	state = json_tokener_state_string_escape;
      }
      this->pos++;
      break;

    case json_tokener_state_string_escape:
      switch(c) {
      case '"':
      case '\\':
      case '/':
	printbuf_memappend(this->pb, this->source + start_offset,
			   this->pos - start_offset - 1);
	start_offset = this->pos++;
	state = saved_state;
	break;
      case 'b':
      case 'n':
      case 'r':
      case 't':
	printbuf_memappend(this->pb, this->source + start_offset,
			   this->pos - start_offset - 1);
	if(c == 'b') printbuf_memappend(this->pb, "\b", 1);
	else if(c == 'n') printbuf_memappend(this->pb, "\n", 1);
	else if(c == 'r') printbuf_memappend(this->pb, "\r", 1);
	else if(c == 't') printbuf_memappend(this->pb, "\t", 1);
	start_offset = ++this->pos;
	state = saved_state;
	break;
      case 'u':
	printbuf_memappend(this->pb, this->source + start_offset,
			   this->pos - start_offset - 1);
	start_offset = ++this->pos;
	state = json_tokener_state_escape_unicode;
	break;
      default:
	*err = json_tokener_error_parse_string;
	goto out;
      }
      break;

    case json_tokener_state_escape_unicode:
      if(strchr(json_hex_chars, c)) {
	this->pos++;
	if(this->pos - start_offset == 4) {
	  unsigned char utf_out[3];
	  unsigned int ucs_char =
	    (hexdigit(*(this->source + start_offset)) << 12) +
	    (hexdigit(*(this->source + start_offset + 1)) << 8) +
	    (hexdigit(*(this->source + start_offset + 2)) << 4) +
	    hexdigit(*(this->source + start_offset + 3));
	  if (ucs_char < 0x80) {
	    utf_out[0] = ucs_char;
	    printbuf_memappend(this->pb, (char*)utf_out, 1);
	  } else if (ucs_char < 0x800) {
	    utf_out[0] = 0xc0 | (ucs_char >> 6);
	    utf_out[1] = 0x80 | (ucs_char & 0x3f);
	    printbuf_memappend(this->pb, (char*)utf_out, 2);
	  } else {
	    utf_out[0] = 0xe0 | (ucs_char >> 12);
	    utf_out[1] = 0x80 | ((ucs_char >> 6) & 0x3f);
	    utf_out[2] = 0x80 | (ucs_char & 0x3f);
	    printbuf_memappend(this->pb, (char*)utf_out, 3);
	  }
	  start_offset = this->pos;
	  state = saved_state;
	}
      } else {
	*err = json_tokener_error_parse_string;
	goto out;
      }
      break;

    case json_tokener_state_boolean:
      if(strncasecmp("true", this->source + start_offset,
		 this->pos - start_offset) == 0) {
	if(this->pos - start_offset == 4) {
	  current = json_object_new_boolean(1);
	  saved_state = json_tokener_state_finish;
	  state = json_tokener_state_eatws;
	} else {
	  this->pos++;
	}
      } else if(strncasecmp("false", this->source + start_offset,
			this->pos - start_offset) == 0) {
	if(this->pos - start_offset == 5) {
	  current = json_object_new_boolean(0);
	  saved_state = json_tokener_state_finish;
	  state = json_tokener_state_eatws;
	} else {
	  this->pos++;
	}
      } else {
	*err = json_tokener_error_parse_boolean;
	goto out;
      }
      break;

    case json_tokener_state_number:
      if(!c || !strchr(json_number_chars, c)) {
	int numi;
	double numd;
	char *tmp = strndup(this->source + start_offset,
			    this->pos - start_offset);
	if(!deemed_double && sscanf(tmp, "%d", &numi) == 1) {
	  current = json_object_new_int(numi);
	} else if(deemed_double && sscanf(tmp, "%lf", &numd) == 1) {
	  current = json_object_new_double(numd);
	} else {
	  free(tmp);
	  *err = json_tokener_error_parse_number;
	  goto out;
	}
	free(tmp);
	saved_state = json_tokener_state_finish;
	state = json_tokener_state_eatws;
      } else {
	if(c == '.' || c == 'e') deemed_double = 1;
	this->pos++;
      }
      break;

    case json_tokener_state_array:
      if(c == ']') {
	this->pos++;
	saved_state = json_tokener_state_finish;
	state = json_tokener_state_eatws;
      } else {
	obj = json_tokener_do_parse(this, err);
	if(*err != json_tokener_success) {
	  *err = json_tokener_error_parse_array;
	  goto out;
	}
	json_object_array_add(current, obj);
	saved_state = json_tokener_state_array_sep;
	state = json_tokener_state_eatws;
      }
      break;

    case json_tokener_state_array_sep:
      if(c == ']') {
	this->pos++;
	saved_state = json_tokener_state_finish;
	state = json_tokener_state_eatws;
      } else if(c == ',') {
	this->pos++;
	saved_state = json_tokener_state_array;
	state = json_tokener_state_eatws;
      } else {
	json_object_put(current);
        *err = json_tokener_error_parse_array;
        return NULL;
      }
      break;

    case json_tokener_state_object:
      state = json_tokener_state_object_field_start;
      start_offset = this->pos;
      break;

    case json_tokener_state_object_field_start:
      if(c == '}') {
	this->pos++;
	saved_state = json_tokener_state_finish;
	state = json_tokener_state_eatws;
      } else if (c == '"' || c == '\'') {
	quote_char = c;
	printbuf_reset(this->pb);
	state = json_tokener_state_object_field;
	start_offset = ++this->pos;
      } else {
	*err = json_tokener_error_parse_object;
	goto out;
      }
      break;

    case json_tokener_state_object_field:
      if(c == quote_char) {
	printbuf_memappend(this->pb, this->source + start_offset,
			   this->pos - start_offset);
	obj_field_name = strdup(this->pb->buf);
	saved_state = json_tokener_state_object_field_end;
	state = json_tokener_state_eatws;
      } else if(c == '\\') {
	saved_state = json_tokener_state_object_field;
	state = json_tokener_state_string_escape;
      }
      this->pos++;
      break;

    case json_tokener_state_object_field_end:
      if(c == ':') {
	this->pos++;
	saved_state = json_tokener_state_object_value;
	state = json_tokener_state_eatws;
      } else {
        *err = json_tokener_error_parse_object;
        return NULL;
      }
      break;

    case json_tokener_state_object_value:
      obj = json_tokener_do_parse(this, err);
      if(*err != json_tokener_success) {
	*err = json_tokener_error_parse_object;
	goto out;
      }
      json_object_object_add(current, obj_field_name, obj);
      free(obj_field_name);
      obj_field_name = NULL;
      saved_state = json_tokener_state_object_sep;
      state = json_tokener_state_eatws;
      break;

    case json_tokener_state_object_sep:
      if(c == '}') {
	this->pos++;
	saved_state = json_tokener_state_finish;
	state = json_tokener_state_eatws;
      } else if(c == ',') {
	this->pos++;
	saved_state = json_tokener_state_object;
	state = json_tokener_state_eatws;
      } else {
	*err = json_tokener_error_parse_object;
	goto out;
      }
      break;

    }
  } while(c);

  if(state != json_tokener_state_finish &&
     saved_state != json_tokener_state_finish)
    *err = json_tokener_error_parse_eof;

 out:
  free(obj_field_name);
  if(*err == json_tokener_success) return current;
  mc_debug("json_tokener_do_parse: error=%d state=%d char=%c\n",
	   *err, state, c);
  json_object_put(current);
  return NULL;
}
