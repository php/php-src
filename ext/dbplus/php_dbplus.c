/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000, 2001 The PHP Group             |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Hartmut Holzgraefe <hartmut@six.de>                         |
   |                                                                      |
   +----------------------------------------------------------------------+
 */

#include "php.h"
#include "php_ini.h"
#include "php_dbplus.h"

#include <saccess.h>
#include <relation.h>
#include <dblight.h>



#define _STRING(x) ((*x)->value.str.val)
#define _INT(x)    ((*x)->value.lval)
#define _HASH(x)   ((*x)->value.ht)

#define DBPLUS_FETCH_RESOURCE(r, z)   ZEND_FETCH_RESOURCE(r, relf *, z, -1, "dbplus_relation", le_dbplus_relation); \
                                    if(!r) RETURN_LONG(ERR_UNKNOWN);


static int
var2tuple(relf *r, zval **zv, tuple *t)
{
  register attribute *ap ;
  unsigned deg ;
  zval **element;
  
  if ((*zv)->type!=IS_ARRAY)
    return 1;
  
  rtupinit(r, t);
  
  ap = r->r_atts;
  deg = r->r_rel.rel_deg;
  do {
    if(SUCCESS!=zend_hash_find((*zv)->value.ht, ap->att_name, strlen(ap->att_name)+1, (void **)&element))
      continue;

    if (! *element) return 1;
    
    switch(ap->att_type) {
    case FT_SHORT:   
      convert_to_long_ex(element);
      AFFIX(ap, t)->f_short    = (short) (*element)->value.lval; 
      break;
    case FT_UNSIGNED:     
      convert_to_long_ex(element);
      AFFIX(ap, t)->f_unsigned = (unsigned) (*element)->value.lval; 
      break;
    case FT_SEQUENCE:
    case FT_LONG:         
      convert_to_long_ex(element);
      AFFIX(ap, t)->f_long     = (long) (*element)->value.lval; 
      break;
    case FT_DATE:         
      convert_to_long_ex(element);
      AFFIX(ap, t)->f_date     = (long) (*element)->value.lval; 
      break;
    case FT_FLOAT:        
      convert_to_double_ex(element);
      AFFIX(ap, t)->f_float    = (float) (*element)->value.dval; 
      break;
    case FT_DOUBLE:       
      convert_to_double_ex(element);
      AFFIX(ap, t)->f_double   = (double) (*element)->value.dval; 
      break;
    case FT_STRING: case FT_DEUTSCH: case FT_CHAR:      
        convert_to_string_ex(element);
        afput(ap, t, (field *)0, (*element)->value.str.val); 
        break;
    case FT_TIMESTAMP:
      /* TODO
      str2timestamp(SvPV(sv, na), (timestamp *)AFFIX(ap, t));
      */
      break;
    case FT_BINARY:
    case FT_BLOB:
    case FT_MEMO:
      break;
    }
  } while (ap++, --deg);
  return 0;
} 



static int
tuple2var(relf * r, tuple * t, zval **zv)
{
    register attribute *ap ;
    unsigned deg ;
    char buf[20];
    zval *element;
    
    zval_dtor(*zv);
    if (array_init(*zv) == FAILURE) {
      return 1;
    }

    ap = r->r_atts;
    deg = r->r_rel.rel_deg;
    do {
      MAKE_STD_ZVAL(element); element->type=IS_NULL;

      switch(ap->att_type) {
      case  FT_SHORT: 
        ZVAL_LONG(element, AFFIX(ap, t)->f_short); 
        break;

      case  FT_UNSIGNED:
        ZVAL_LONG(element, AFFIX(ap, t)->f_unsigned); 
        break;

      case  FT_LONG:  
      case  FT_SEQUENCE:
        ZVAL_LONG(element, AFFIX(ap, t)->f_long); 
        break;

      case  FT_FLOAT:  
        ZVAL_DOUBLE(element, AFFIX(ap, t)->f_float); 
        break;

      case  FT_DOUBLE:  
        ZVAL_DOUBLE(element, AFFIX(ap, t)->f_double); 
        break;

      case FT_STRING: 
      case FT_DEUTSCH: 
      case FT_CHAR:
        ZVAL_STRING(element, AFVAR(ap, t)->f_string, 1);
        break;

      }

      if(element->type!=IS_NULL)
        zend_hash_update((*zv)->value.ht,
                 ap->att_name,
                 strlen(ap->att_name)+1,
                 (void *)&element,
                 sizeof(zval*),
                 NULL);
  
    } while (ap++, --deg);
    return 0;
} 

static constraint *
ary2constr(relf * r, zval** constr)
{
  attribute *ap;
  static constraint c;
  int len;
  int alen;
  field *f;
  enum scalop sop;
  int n_elems;
  int n;
  char * dom;
  char * val;
  char * op;
  zval **zdata;

  /* init first */
  db_coninit(r, &c);

  if ((*constr)->type != IS_ARRAY) {
    php_error(E_WARNING, "Constraint is not an array");
    return NULL;
  }

  zend_hash_internal_pointer_reset(_HASH(constr));
  if(zend_hash_get_current_data(_HASH(constr), (void **)&zdata)!=SUCCESS) {
    php_error(E_WARNING, "Constraint array is empty");
    return NULL;
  }
  
  switch((*zdata)->type) {
  case IS_STRING: /* constraints in plain string array */
    if (_HASH(constr)->nNumOfElements%3) {
      php_error(E_WARNING, "Constraint array has to have triples of strings");
      return NULL;
    }

    do {
      convert_to_string_ex(zdata);
      dom = _STRING(zdata);
      zend_hash_move_forward(_HASH(constr));
      zend_hash_get_current_data(_HASH(constr), (void **)&zdata);
      convert_to_string_ex(zdata);
      op  = _STRING(zdata);
      zend_hash_move_forward(_HASH(constr));
      zend_hash_get_current_data(_HASH(constr), (void **)&zdata);
      convert_to_string_ex(zdata);
      val = _STRING(zdata);
      zend_hash_move_forward(_HASH(constr));
      
      if (!(ap = (attribute *) attno (r, dom))) {
        fprintf(stderr, "Invalid domain \"%s\"\n", dom);
        return 0;
      }

      /* operator */
      string_to_scalop(op, &sop);
      
      /* value */
      f = string_to_field(val, ap, 0);
      
      (void) db_constrain(r, &c, dom, sop, f ? f : (field *) val);
    } while(SUCCESS==zend_hash_get_current_data(_HASH(constr), (void **)&zdata));
    
    break;
  case IS_ARRAY:
    {
      zval **entry;
      for(zend_hash_internal_pointer_reset(_HASH(constr));
          SUCCESS==zend_hash_get_current_data(_HASH(constr), (void **)&zdata);
          zend_hash_move_forward(_HASH(constr))) {
        if(!((*zdata)->type==IS_ARRAY)) {
          php_error(E_WARNING, "Constraint array element not an array");
          return NULL;
        }
        if(_HASH(zdata)->nNumOfElements!=3) {
          php_error(E_WARNING, "Constraint array element not an array of size 3");
          return NULL;
        }       

        zend_hash_internal_pointer_reset(_HASH(zdata));
        zend_hash_get_current_data(_HASH(zdata), (void **)&entry);
        convert_to_string_ex(entry);
        dom=_STRING(entry);

        zend_hash_move_forward(_HASH(zdata));
        zend_hash_get_current_data(_HASH(zdata), (void **)&entry);
        convert_to_string_ex(entry);
        op=_STRING(entry);

        zend_hash_move_forward(_HASH(zdata));       
        zend_hash_get_current_data(_HASH(zdata), (void **)&entry);
        convert_to_string_ex(entry);
        val=_STRING(entry);

        if (!(ap = (attribute *) attno (r, dom))) {
          fprintf(stderr, "Invalid domain \"%s\"\n", dom);
          return 0;
        }

        /* operator */
        string_to_scalop(op, &sop);
        
        /* value */
        f = string_to_field(val, ap, 0);

        (void) db_constrain(r, &c, dom, sop, f ? f : (field *) val);
      }
              
    }
    break;
  default:
    /* TODO error-handling */
    return NULL;
  }

  return &c;
}


/* {{{ proto int dbplus_add(int relation, array tuple)
   Add a tuple to a relation */
PHP_FUNCTION(dbplus_add)
{
  zval **relation, **data, **element;
  enum errorcond stat = ERR_UNKNOWN;
  relf *r;
  tuple t;


  if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &relation, &data) == FAILURE){
    WRONG_PARAM_COUNT;
  }

  DBPLUS_FETCH_RESOURCE(r, relation);
  convert_to_array_ex(data);
  
  if(var2tuple(r, data, &t))
    RETURN_LONG(ERR_UNKNOWN);

  stat=cdb_add(r, &t);
  if(stat==ERR_NOERR) {
    tuple2var(r, &t, data);
  }

  RETURN_LONG(stat);
}
/* }}} */

/* {{{ proto int dbplus_aql(string query [, string server [, string dbpath]])
   Perform AQL query */
PHP_FUNCTION(dbplus_aql)
{
  int argc;
  zval **query, **server, **dbpath;
  relf *r;

  argc = ZEND_NUM_ARGS();
  if (argc < 1 || argc > 3 || zend_get_parameters_ex(argc, &query, &server, &dbpath) == FAILURE){
    WRONG_PARAM_COUNT;
  }
  
  switch (argc) {
  case 3:
    convert_to_string_ex(dbpath);
    php_error(E_WARNING, "Arg dbpath: %s", _STRING(dbpath));
    /* Fall-through. */
  case 2:
    convert_to_string_ex(server);
    php_error(E_WARNING, "Arg server: %s", _STRING(server));
    /* Fall-through. */
  case 1:
    convert_to_string_ex(query);
    php_error(E_WARNING, "Arg query: %s", _STRING(query));
    break;
  }
  
  r = cdb_aql((argc>=2)?_STRING(server):"localhost",
                 _STRING(query),
                 (argc==3)?_STRING(dbpath):NULL);
  if(r == NULL) {
    /* TODO error handling */
    RETURN_FALSE;
  }
  
  ZEND_REGISTER_RESOURCE(return_value, r, le_dbplus_relation);  
}
/* }}} */

/* {{{ proto string dbplus_chdir([string newdir])
   Get/Set database virtual current directory */
PHP_FUNCTION(dbplus_chdir)
{
  int argc;
  char *p;
  zval **newdir;

  argc = ZEND_NUM_ARGS();
  switch(argc) {
  case 0: 
    break;
  case 1:
    if(zend_get_parameters_ex(1, &newdir) == FAILURE) {
      WRONG_PARAM_COUNT;
    } else {
      convert_to_string_ex(newdir);
    }
    break;
  default:
    WRONG_PARAM_COUNT;
  }

  p = cdb_chdir((argc)?_STRING(newdir):NULL);
  if(p) {
    RETURN_STRING(p, 1);
  } else {
    RETURN_FALSE;
  }
}
/* }}} */

/* {{{ proto int dbplus_close(int relation)
   Close a relation */
PHP_FUNCTION(dbplus_close)
{
  relf *r;
  zval **relation;
  if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &relation) == FAILURE){
    WRONG_PARAM_COUNT;
  }

  DBPLUS_FETCH_RESOURCE(r, relation);

  zend_list_delete((*relation)->value.lval);  

  RETURN_TRUE;
}
/* }}} */

/* {{{ proto int dbplus_curr(int relation, array tuple)
   Get current tuple from relation */
PHP_FUNCTION(dbplus_curr)
{
  zval **relation, **tname;
  relf *r;
  tuple t;
  int stat;

  if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &relation, &tname) == FAILURE){
    WRONG_PARAM_COUNT;
  }

  DBPLUS_FETCH_RESOURCE(r, relation);
  
  stat = cdb_current(r, &t);
  if(stat==ERR_NOERR) {
    tuple2var(r, &t, tname);
  }

  RETURN_LONG(stat);
}
/* }}} */

/* {{{ proto string dbplus_errcode(int err)
   Get error string for given errorcode or last error */
PHP_FUNCTION(dbplus_errcode)
{
  zval **err;
  int errno;
  
  switch (ZEND_NUM_ARGS()) {
  case 0:
    errno=-1;
    break;
  case 1:
    if( zend_get_parameters_ex(1, &err) == FAILURE){
      WRONG_PARAM_COUNT;
    }
    convert_to_long_ex(err);
    errno = _INT(err);
  }
  
  if(errno==-1) errno = Acc_error;

  RETURN_STRING(dbErrorMsg(errno, NULL), 1); 
}
/* }}} */

/* {{{ proto int dbplus_errno(void)
   Get error code for last operation */
PHP_FUNCTION(dbplus_errno)
{
  RETURN_LONG(Acc_error);
}
/* }}} */

/* {{{ proto int dbplus_find(int relation, array constr, mixed tuple)
   Set a constraint on a relation*/
PHP_FUNCTION(dbplus_find)
{
  relf *r;
  zval **relation, **constr, **data;
  constraint *c;
  tuple t;
  int stat;

  if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &relation, &constr, &data) == FAILURE){
    WRONG_PARAM_COUNT;
  }

  DBPLUS_FETCH_RESOURCE(r, relation); 

  if ((*constr)->type != IS_ARRAY) {
    php_error(E_WARNING, "Constraint is not an array");
    RETURN_LONG(ERR_UNKNOWN);
  }

  convert_to_array_ex(data);

  c = ary2constr(r, constr);
  
  if (!c){
    RETURN_LONG(ERR_USER);
  }

  stat = cdb_find(r, &t, c);
  
  if(stat==ERR_NOERR)
    tuple2var(r, &t, data); 

  RETURN_LONG(stat);
}
/* }}} */

/* {{{ proto int dbplus_first(int relation, array tuple)
   Get first tuple from relation */
PHP_FUNCTION(dbplus_first)
{
  zval **relation, **tname;
  relf *r;
  tuple t;
  int stat;

  if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &relation, &tname) == FAILURE){
    WRONG_PARAM_COUNT;
  }

  DBPLUS_FETCH_RESOURCE(r, relation);
  
  stat = cdb_first(r, &t);
  if(stat==ERR_NOERR) {
    tuple2var(r, &t, tname);
  }

  RETURN_LONG(stat);
}
/* }}} */

/* {{{ proto int dbplus_flush(int relation)
   ??? */
PHP_FUNCTION(dbplus_flush)
{
  relf *r;
  zval **relation;
  if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &relation) == FAILURE){
    WRONG_PARAM_COUNT;
  }

  DBPLUS_FETCH_RESOURCE(r, relation);

  RETURN_LONG(cdb_flush(r));
}
/* }}} */

/* {{{ proto int dbplus_freealllocks(void)
   Free all locks held by this client */
PHP_FUNCTION(dbplus_freealllocks)
{
  RETURN_LONG(cdbFreeAllLocks());
}
/* }}} */


/* {{{ proto int dbplus_freelock(int relation, array tuple)
   Release write lock on tuple */
PHP_FUNCTION(dbplus_freelock)
{
  zval **relation, **data, **element;
  enum errorcond stat = ERR_UNKNOWN;
  relf *r;
  tuple t;

  if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &relation, &data) == FAILURE){
    WRONG_PARAM_COUNT;
  }

  DBPLUS_FETCH_RESOURCE(r, relation);

  convert_to_array_ex(data);
  
  if(var2tuple(r, data, &t))
    RETURN_LONG(ERR_UNKNOWN);

  stat=cdb_freelock(r, &t);

  RETURN_LONG(stat);
}
/* }}} */

/* {{{ proto int dbplus_freerlocks(int relation)
   Free all locks on given relation */
PHP_FUNCTION(dbplus_freerlocks)
{
  relf *r;
  zval **relation;
  if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &relation) == FAILURE){
    WRONG_PARAM_COUNT;
  }

  DBPLUS_FETCH_RESOURCE(r, relation);

  RETURN_LONG(cdb_freerlocks(r));
}
/* }}} */

/* {{{ proto int dbplus_getlock(int relation, array tuple)
   Request locking of tuple */
PHP_FUNCTION(dbplus_getlock)
{
  zval **relation, **data, **element;
  enum errorcond stat = ERR_UNKNOWN;
  relf *r;
  tuple t;

  if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &relation, &data) == FAILURE){
    WRONG_PARAM_COUNT;
  }

  DBPLUS_FETCH_RESOURCE(r, relation);

  convert_to_array_ex(data);
  
  if(var2tuple(r, data, &t))
    RETURN_LONG(ERR_UNKNOWN);

  stat=cdb_getlock(r, &t);

  RETURN_LONG(stat);
}
/* }}} */

/* {{{ proto int dbplus_getunique(int handle, int uniqueid)
   Get a id number unique to a relation */
PHP_FUNCTION(dbplus_getunique)
{
  relf *r;
  zval **relation, **uniqueid;
  long l;
  int stat;
  
  if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &relation, &uniqueid) == FAILURE){
    WRONG_PARAM_COUNT;
  }
  
  DBPLUS_FETCH_RESOURCE(r, relation);
  
  stat = cdb_getunique(r, &l, 1);
  if(!stat) {
    ZVAL_LONG(*uniqueid,l);
  } 

  RETURN_LONG(stat);
}
/* }}} */

/* {{{ proto int dbplus_info(int relation, string key, array &result)
   ??? */
PHP_FUNCTION(dbplus_info)
{
  zval **relation, **key, **result, *element;
  relf *r;
  register attribute *ap;
  unsigned deg;

  if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &relation, &key, &result) == FAILURE){
    WRONG_PARAM_COUNT;
  }

  DBPLUS_FETCH_RESOURCE(r, relation);
  
  ap = r->r_atts;
  deg = r->r_rel.rel_deg;

  convert_to_string_ex(key);
  
  zval_dtor(*result);
  if (array_init(*result) == FAILURE)
    RETURN_LONG(ERR_USER);  

  if(!strcmp("atts", (*key)->value.str.val)) {
    do {
      MAKE_STD_ZVAL(element); 

      ZVAL_STRING(element, ap->att_name, 1);

      zend_hash_update((*result)->value.ht,
                       ap->att_name,
                       strlen(ap->att_name)+1,
                       (void *)&element,
                       sizeof(zval*),
                       NULL);
    } while (ap++, deg--);
    RETURN_LONG(ERR_NOERR);
  }

  RETURN_LONG(ERR_USER);
}
/* }}} */

/* {{{ proto int dbplus_last(int relation, array tuple)
   Get last tuple from relation */
PHP_FUNCTION(dbplus_last)
{
  zval **relation, **tname;
  relf *r;
  tuple t;
  int stat;

  if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &relation, &tname) == FAILURE){
    WRONG_PARAM_COUNT;
  }

  DBPLUS_FETCH_RESOURCE(r, relation);
  
  stat = cdb_last(r, &t);
  if(stat==ERR_NOERR) {
    tuple2var(r, &t, tname);
  }

  RETURN_LONG(stat);
}
/* }}} */


/* {{{ proto int dbplus_lockrel(int relation)
   Request write lock on relation */
PHP_FUNCTION(dbplus_lockrel)
{
  relf *r;
  zval **relation;
  if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &relation) == FAILURE){
    WRONG_PARAM_COUNT;
  }

  DBPLUS_FETCH_RESOURCE(r, relation);

  RETURN_LONG(cdb_lockrel(r));
}
/* }}} */

/* {{{ proto int dbplus_next(int relation, array &tname)
   Get next tuple from relation */
PHP_FUNCTION(dbplus_next)
{
  zval **relation, **tname, *element;
  relf *r;
  tuple t;
  int stat;
  
  if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &relation, &tname) == FAILURE){
    WRONG_PARAM_COUNT;
  }
  
  DBPLUS_FETCH_RESOURCE(r, relation);
  
  stat = cdb_next(r, &t);
  if(stat==ERR_NOERR) {
    tuple2var(r, &t, tname);
  }

  RETURN_LONG(stat);
}
/* }}} */

/* {{{ proto int dbplus_open(string name)
   Open a relation file */
PHP_FUNCTION(dbplus_open)
{
  relf *r;
  zval **tname;
  if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &tname) == FAILURE){
    WRONG_PARAM_COUNT;
  }

  convert_to_string_ex(tname);

  r = cdb_open((*tname)->value.str.val, 1, 1);
  if(r == NULL) {
    /* TODO error handling */
    RETURN_FALSE;
  }

  ZEND_REGISTER_RESOURCE(return_value, r, le_dbplus_relation);
}
/* }}} */

/* {{{ proto int dbplus_prev(int relation, array tuple)
   Get previous tuple from relation */
PHP_FUNCTION(dbplus_prev)
{
  zval **relation, **tname;
  relf *r;
  tuple t;
  int stat;

  if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &relation, &tname) == FAILURE){
    WRONG_PARAM_COUNT;
  }

  DBPLUS_FETCH_RESOURCE(r, relation);
  
  stat = cdb_previous(r, &t);
  if(stat==ERR_NOERR) {
    tuple2var(r, &t, tname);
  }

  RETURN_LONG(stat);
}
/* }}} */

/* {{{ proto int dbplus_rchperm(int relation, int mask, string user, string group)
    */
PHP_FUNCTION(dbplus_rchperm)
{
  relf *r;
  zval **relation, **mask, **user, **group;
  if (ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &relation, &mask, &user, &group) == FAILURE){
    WRONG_PARAM_COUNT;
  }

  DBPLUS_FETCH_RESOURCE(r, relation);

  convert_to_long_ex(mask);
  convert_to_string_ex(user);
  convert_to_string_ex(group);

  RETURN_LONG(cdbRchperm(r, _INT(mask), _STRING(user), _STRING(group)));
}
/* }}} */

/* {{{ proto int dbplus_rcreate(string name, string domlist [, int overwrite])
    */
PHP_FUNCTION(dbplus_rcreate)
{
  zval **name, **domlist, **overwrite;
  relf *r=NULL;
  int flag, ndoms, argc = ZEND_NUM_ARGS();
  attdef *at0;

  switch(argc) {
  case 3:
    if(zend_get_parameters_ex(3, &name, &domlist, &overwrite) == FAILURE) {
      WRONG_PARAM_COUNT;
    }     
    convert_to_long_ex(overwrite);
    flag=_INT(overwrite);
    break;
  case 2:
    if(zend_get_parameters_ex(3, &name, &domlist) == FAILURE) {
      WRONG_PARAM_COUNT;
    }
    flag=0;
    break;
  default:
    WRONG_PARAM_COUNT;
    break;
  }

  convert_to_string_ex(name);
  convert_to_string_ex(domlist);

  at0 = create2att(_STRING(domlist), &ndoms);
  if (at0) {
    r = cdbRcreate(_STRING(name), 0666, 0, ndoms, at0, flag);
    dbxfree((char *) at0);
  }
  if(r == NULL) {
    /* TODO error handling */
    RETURN_FALSE;
  }

  ZEND_REGISTER_RESOURCE(return_value, r, le_dbplus_relation);
}
/* }}} */

/* {{{ proto int dbplus_rcrtexact(string name, int handle[, int overwrite])
    */
PHP_FUNCTION(dbplus_rcrtexact)
{
  zval **name, **relation, **overwrite;
  relf *r;
  int f,argc = ZEND_NUM_ARGS();

  switch(argc) {
  case 3:
    if(zend_get_parameters_ex(3, &name, &relation, &overwrite) == FAILURE) {
      WRONG_PARAM_COUNT;
    }     
    convert_to_long_ex(overwrite);
    f=_INT(overwrite);
    break;
  case 2:
    if(zend_get_parameters_ex(3, &name, &relation) == FAILURE) {
      WRONG_PARAM_COUNT;
    }
    f=0;
    break;
  default:
    WRONG_PARAM_COUNT;
    break;
  }

  convert_to_string_ex(name);
  DBPLUS_FETCH_RESOURCE(r, relation);

  r = cdbRcrtexact(_STRING(name), 0666, r, f);
  if(r == NULL) {
    /* TODO error handling */
    RETURN_FALSE;
  }

  ZEND_REGISTER_RESOURCE(return_value, r, le_dbplus_relation);
}
/* }}} */

/* {{{ proto int dbplus_rcrtlike(string name, int handle [, int overwrite])
    */
PHP_FUNCTION(dbplus_rcrtlike)
{
  zval **name, **relation, **overwrite;
  relf *r;
  int f,argc = ZEND_NUM_ARGS();

  switch(argc) {
  case 3:
    if(zend_get_parameters_ex(3, &name, &relation, &overwrite) == FAILURE) {
      WRONG_PARAM_COUNT;
    }     
    convert_to_long_ex(overwrite);
    f=_INT(overwrite);
    break;
  case 2:
    if(zend_get_parameters_ex(3, &name, &relation) == FAILURE) {
      WRONG_PARAM_COUNT;
    }
    f=0;
    break;
  default:
    WRONG_PARAM_COUNT;
    break;
  }

  convert_to_string_ex(name);
  DBPLUS_FETCH_RESOURCE(r, relation);

  r = cdbRcrtlike(_STRING(name), 0666, 0, r, f);
  if(r == NULL) {
    /* TODO error handling */
    RETURN_FALSE;
  }

  ZEND_REGISTER_RESOURCE(return_value, r, le_dbplus_relation);
}
/* }}} */

/* {{{ proto int dbplus_resolve(string name)
   Resolve host information for relation */
PHP_FUNCTION(dbplus_resolve)
{
  zval **name, *element;
  char * host;
  char * host_path;
  int sid;

  if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &name) == FAILURE){
    WRONG_PARAM_COUNT;
  }

  convert_to_string_ex(name);

  sid = cdb_resolve(_STRING(name), &host, &host_path);
  if (sid <= 0)
    RETURN_FALSE;

  if (array_init(return_value) == FAILURE) {
    RETURN_FALSE;
  }


  MAKE_STD_ZVAL(element); element->type=IS_NULL;
  ZVAL_LONG(element,sid);
  zend_hash_update(Z_ARRVAL_P(return_value), "sid", 4,
            &element, sizeof(zval *), NULL);

  MAKE_STD_ZVAL(element); element->type=IS_NULL;
  ZVAL_STRING(element,host,1);
  zend_hash_update(Z_ARRVAL_P(return_value), "host", 5,
            &element, sizeof(zval *), NULL);

  MAKE_STD_ZVAL(element); element->type=IS_NULL;
  ZVAL_STRING(element,host_path,1);
  zend_hash_update(Z_ARRVAL_P(return_value), "host_path", 10,
            &element, sizeof(zval *), NULL);
}
/* }}} */

/* {{{ proto int dbplus_restorepos(int relation, array tuple)
   ??? */
PHP_FUNCTION(dbplus_restorepos)
{
  zval **relation, **tname;
  relf *r;
  tuple t;
  int stat;
  if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &relation, &tname) == FAILURE){
    WRONG_PARAM_COUNT;
  }

  DBPLUS_FETCH_RESOURCE(r, relation);
  
  stat = cdb_next(r, &t);
  if(stat==ERR_NOERR) {
    tuple2var(r, &t, tname);
  }

  RETURN_LONG(stat);
}
/* }}} */

/* {{{ proto int dbplus_rkeys(int relation, [string|array] domlist)
    */
PHP_FUNCTION(dbplus_rkeys)
{
  relf *r, *rnew;
  zval **relation, **domlist, **zdata;
  int nkeys=0;
  char *p, *name=NULL, *keys[40]; /* TODO hardcoded magic number ??? */

  if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &relation, &domlist) == FAILURE){
    WRONG_PARAM_COUNT;
  }

  DBPLUS_FETCH_RESOURCE(r, relation);

  if((*domlist)->type == IS_ARRAY) {
    convert_to_array_ex(domlist);
    for(zend_hash_internal_pointer_reset(_HASH(domlist));
        SUCCESS==zend_hash_get_current_data(_HASH(domlist), (void **)&zdata);
        zend_hash_move_forward(_HASH(domlist))) {
      if((*zdata)->type==IS_STRING)
        keys[nkeys++] = _STRING(zdata);
      else {
        php_error(E_WARNING, "Domlist array contains non-string value(s)");
        Acc_error = ERR_USER;
        RETURN_FALSE;
      }
    }
  } else {
    convert_to_string_ex(domlist);
    name = estrdup(_STRING(domlist));
    while (p = strtok(nkeys ? 0 : name, " \t"))
      keys[nkeys++] = p;
  }
  
  rnew = cdbRkeys(r, nkeys, keys);
  if(name) efree(name);

  if(rnew) {
    /* TODO realy delete old relation resource ? */
    zend_list_delete((*relation)->value.lval);  
    
    ZEND_REGISTER_RESOURCE(return_value, rnew, le_dbplus_relation);
  } else {
    /* TODO error reporting */
    RETURN_FALSE;
  }
}
/* }}} */

/* {{{ proto int dbplus_ropen(string name)
   Open relation file ... ??? */
PHP_FUNCTION(dbplus_ropen)
{
  relf *r;
  zval **tname;
  if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &tname) == FAILURE){
    WRONG_PARAM_COUNT;
  }

  convert_to_string_ex(tname);

  r = ropen((*tname)->value.str.val, 0, 0);
  if(r == NULL) {
    /* TODO error handling */
    RETURN_FALSE;
  }

  ZEND_REGISTER_RESOURCE(return_value, r, le_dbplus_relation);
}
/* }}} */

/* {{{ proto int dbplus_rquery(string name, string dbpath)
    */
PHP_FUNCTION(dbplus_rquery)
{
  relf *r;
  zval **name, **dbpath;
  int argc;
  
  if (argc <1 || argc>2 || zend_get_parameters_ex(1, &name, &dbpath) == FAILURE){
    WRONG_PARAM_COUNT;
  }

  r = aql_exec(_STRING(name), (argc==2)?_STRING(dbpath):NULL);

  if(!r) {
    /* TODO error handling */
    RETURN_FALSE;
  }

  ZEND_REGISTER_RESOURCE(return_value, r, le_dbplus_relation);
}
/* }}} */

/* {{{ proto int dbplus_rrename(int relation, string name)
    */
PHP_FUNCTION(dbplus_rrename)
{
  relf *r;
  zval **relation, **name;
  if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &relation, &name) == FAILURE){
    WRONG_PARAM_COUNT;
  }

  DBPLUS_FETCH_RESOURCE(r, relation);

  convert_to_string_ex(name);

  RETURN_LONG(cdbRrename(r, _STRING(name), 0));
}
/* }}} */

/* {{{ proto int dbplus_rsecindex(int relation, string domlist, int compact)
    */
PHP_FUNCTION(dbplus_rsecindex)
{
  relf *r, *rnew;
  zval **relation, **domlist, **compact, **zdata;
  int nkeys=0;
  char *p, *name=NULL, *keys[40]; /* TODO hardcoded magic number ??? */

  if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &relation, &domlist, &compact) == FAILURE){
    WRONG_PARAM_COUNT;
  }

  DBPLUS_FETCH_RESOURCE(r, relation);

  if((*domlist)->type == IS_ARRAY) {
    convert_to_array_ex(domlist);
    for(zend_hash_internal_pointer_reset(_HASH(domlist));
        SUCCESS==zend_hash_get_current_data(_HASH(domlist), (void **)&zdata);
        zend_hash_move_forward(_HASH(domlist))) {
      if((*zdata)->type==IS_STRING)
        keys[nkeys++] = _STRING(zdata);
      else {
        php_error(E_WARNING, "Domlist array contains non-string value(s)");
        Acc_error = ERR_USER;
        RETURN_FALSE;
      }
    }
  } else {
    convert_to_string_ex(domlist);
    name = estrdup(_STRING(domlist));
    while (p = strtok(nkeys ? 0 : name, "   "))
      keys[nkeys++] = p;
  }

  convert_to_long_ex(compact);
  
  rnew = cdbRsecindex(r, nkeys, keys, _INT(compact));
  if(name) efree(name);

  if(rnew) {
    /* TODO realy delete old relation resource ? */
    zend_list_delete((*relation)->value.lval);  
    
    ZEND_REGISTER_RESOURCE(return_value, rnew, le_dbplus_relation);
  } else {
    /* TODO error reporting */
    RETURN_FALSE;
  }
}
/* }}} */

/* {{{ proto int dbplus_runlink(int relation)
   Remove relation from filesystem */
PHP_FUNCTION(dbplus_runlink)
{
  relf *r;
  zval **relation;
  if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &relation) == FAILURE){
    WRONG_PARAM_COUNT;
  }

  DBPLUS_FETCH_RESOURCE(r, relation);

  RETURN_LONG(cdbRunlink(&r));
}
/* }}} */

/* {{{ proto int dbplus_rzap(int relation, int truncate)
   Remove all tuples from relation */
PHP_FUNCTION(dbplus_rzap)
{

  /* todo: optional argument */
  relf *r;
  zval **relation, **truncate;
  if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &relation) == FAILURE){
    WRONG_PARAM_COUNT;
  }
 
  DBPLUS_FETCH_RESOURCE(r, relation);

  RETURN_LONG(cdbRzap(r, 1));
}
/* }}} */

/* {{{ proto int dbplus_savepos(int relation)
   ??? */
PHP_FUNCTION(dbplus_savepos)
{
  relf *r;
  zval **relation;
  if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &relation) == FAILURE){
    WRONG_PARAM_COUNT;
  }

  DBPLUS_FETCH_RESOURCE(r, relation);

  RETURN_LONG(cdb_savepos(r));
}
/* }}} */

/* {{{ proto int dbplus_setindex(int relation, string idx_name)
   ???? */
PHP_FUNCTION(dbplus_setindex)
{
  relf *r;
  zval **relation, **idx_name;
  if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &relation, &idx_name) == FAILURE){
    WRONG_PARAM_COUNT;
  }

  DBPLUS_FETCH_RESOURCE(r, relation);

  convert_to_string_ex(idx_name);
  
  RETURN_LONG(cdb_setindex(r, _STRING(idx_name)));
}
/* }}} */

/* {{{ proto int dbplus_setindexbynumber(int relation, int idx_number)
   ??? */
PHP_FUNCTION(dbplus_setindexbynumber)
{
  relf *r;
  zval **relation, **idx_number;
  if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &relation, &idx_number) == FAILURE){
    WRONG_PARAM_COUNT;
  }

  DBPLUS_FETCH_RESOURCE(r, relation);

  convert_to_long_ex(idx_number);
  
  RETURN_LONG(cdb_setindexbynumber(r, (*idx_number)->value.lval));
}
/* }}} */

/* {{{ proto int dbplus_sql(string query, string server, string dbpath)
   Perform SQL query */
PHP_FUNCTION(dbplus_sql)
{
  int argc;
  zval **query, **server, **dbpath;
  relf *r;

  argc = ZEND_NUM_ARGS();
  if (argc < 1 || argc > 3 || zend_get_parameters_ex(argc, &query, &server, &dbpath) == FAILURE){
    WRONG_PARAM_COUNT;
  }
  
  switch (argc) {
  case 3:
    convert_to_string_ex(dbpath);
    php_error(E_WARNING, "Arg dbpath: %s", _STRING(dbpath));
    /* Fall-through. */
  case 2:
    convert_to_string_ex(server);
    php_error(E_WARNING, "Arg server: %s", _STRING(server));
    /* Fall-through. */
  case 1:
    convert_to_string_ex(query);
    php_error(E_WARNING, "Arg query: %s", _STRING(query));
    break;
  }
  
  r = cdb_sql((argc>=2)?_STRING(server):"localhost",
                 _STRING(query),
                 (argc==3)?_STRING(dbpath):NULL);
  if(r == NULL) {
    /* TODO error handling */
    RETURN_FALSE;
  }
  
  ZEND_REGISTER_RESOURCE(return_value, r, le_dbplus_relation);  
}
/* }}} */

/* {{{ proto string dbplus_tcl(int sid, string script)
    */
PHP_FUNCTION(dbplus_tcl)
{
  zval **sid, **script;
  char *ret;
  int result_type;

  if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &sid, &script) == FAILURE){
    WRONG_PARAM_COUNT;
  }

  convert_to_long_ex(sid);
  convert_to_string_ex(script);

  cdb_tcl(_INT(sid),_STRING(script),&ret,&result_type);

  if(ret) {
    RETURN_STRING(ret,1);
  } else {
    RETURN_FALSE;
  }
}
/* }}} */

/* {{{ proto int dbplus_tremove(int relation, array old [, array current])
   Remove tuple and return new current tuple */
PHP_FUNCTION(dbplus_tremove)
{
  zval **relation, **old, **current;
  enum errorcond stat = ERR_UNKNOWN;
  relf *r;
  tuple told, tcurr;
  int argc;
  
  argc = ZEND_NUM_ARGS(); 
  if ( argc<2 || argc>3 || zend_get_parameters_ex(2, &relation, &old, &current) == FAILURE){
    WRONG_PARAM_COUNT;
  }

  DBPLUS_FETCH_RESOURCE(r, relation);

  convert_to_array_ex(old);
  
  if(var2tuple(r, old, &told))
    RETURN_LONG(ERR_UNKNOWN);

  stat=cdbTremove(r, &told, &tcurr);
  
  if((stat==ERR_NOERR) && (argc==3))
    tuple2var(r, &tcurr, current);

  RETURN_LONG(stat);
}
/* }}} */

/* {{{ proto int dbplus_undo(int relation)
   ??? */
PHP_FUNCTION(dbplus_undo)
{
  relf *r;
  zval **relation;
  if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &relation) == FAILURE){
    WRONG_PARAM_COUNT;
  }

  DBPLUS_FETCH_RESOURCE(r, relation);

  RETURN_LONG(cdb_undo(r));
}
/* }}} */

/* {{{ proto int dbplus_undoprepare(int relation)
   ??? */
PHP_FUNCTION(dbplus_undoprepare)
{
  relf *r;
  zval **relation;
  if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &relation) == FAILURE){
    WRONG_PARAM_COUNT;
  }

  DBPLUS_FETCH_RESOURCE(r, relation);

  RETURN_LONG(cdb_undoprepare(r));
}
/* }}} */

/* {{{ proto int dbplus_unlockrel(int relation)
   Give up write lock on relation */
PHP_FUNCTION(dbplus_unlockrel)
{
  relf *r;
  zval **relation;
  if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &relation) == FAILURE){
    WRONG_PARAM_COUNT;
  }

  DBPLUS_FETCH_RESOURCE(r, relation);

  RETURN_LONG(cdb_unlockrel(r));
}
/* }}} */

/* {{{ proto int dbplus_unselect(int relation)
   Remove constraint from relation */
PHP_FUNCTION(dbplus_unselect)
{
  relf *r;
  zval **relation;
  if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &relation) == FAILURE){
    WRONG_PARAM_COUNT;
  }

  DBPLUS_FETCH_RESOURCE(r, relation);

  RETURN_LONG(cdb_unselect(r));
}
/* }}} */

/* {{{ proto int dbplus_update(int relation, array old, array new)
   Update specified tuple in relation */
PHP_FUNCTION(dbplus_update)
{
  zval **relation, **old, **new;
  enum errorcond stat = ERR_UNKNOWN;
  relf *r;
  tuple told, tnew;

  if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &relation, &old, &new) == FAILURE){
    WRONG_PARAM_COUNT;
  }

  DBPLUS_FETCH_RESOURCE(r, relation);

  convert_to_array_ex(old);
  convert_to_array_ex(new);
  
  if(var2tuple(r, old, &told))
    RETURN_LONG(ERR_UNKNOWN);

  if(var2tuple(r, new, &tnew))
    RETURN_LONG(ERR_UNKNOWN);

  stat=cdb_update(r, &told, &tnew);
  
  RETURN_LONG(stat);
}
/* }}} */

/* {{{ proto int dbplus_xlockrel(int relation)
   Request exclusive lock on relation */
PHP_FUNCTION(dbplus_xlockrel)
{
  relf *r;
  zval **relation;
  if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &relation) == FAILURE){
    WRONG_PARAM_COUNT;
  }

  DBPLUS_FETCH_RESOURCE(r, relation);

  RETURN_LONG(cdb_xlockrel(r));
}
/* }}} */

/* {{{ proto int dbplus_xunlockrel(int relation)
   Free exclusive lock on relation */
PHP_FUNCTION(dbplus_xunlockrel)
{
  relf *r;
  zval **relation;
  if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &relation) == FAILURE){
    WRONG_PARAM_COUNT;
  }

  DBPLUS_FETCH_RESOURCE(r, relation);

  RETURN_LONG(cdb_xunlockrel(r));
}
/* }}} */

/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
