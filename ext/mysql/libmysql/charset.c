/* Copyright Abandoned 1996 TCX DataKonsult AB & Monty Program KB & Detron HB 
This file is public domain and comes with NO WARRANTY of any kind */

#include "mysys_priv.h"
#include "mysys_err.h"
#include <m_ctype.h>
#include <m_string.h>
#include <my_dir.h>

typedef struct cs_id_st {
  char *name;
  uint number;
} CS_ID;

const char *charsets_dir = NULL;
static DYNAMIC_ARRAY cs_info_table;
static CS_ID **available_charsets;
static int charset_initialized=0;

#define MAX_LINE  1024

#define CTYPE_TABLE_SIZE      257
#define TO_LOWER_TABLE_SIZE   256
#define TO_UPPER_TABLE_SIZE   256
#define SORT_ORDER_TABLE_SIZE 256

struct simpleconfig_buf_st {
  FILE *f;
  char  buf[MAX_LINE];
  char *p;
};

/* Defined in strings/ctype.c */

CHARSET_INFO *find_compiled_charset(uint cs_number);
uint compiled_charset_number(const char *name);
const char *compiled_charset_name(uint charset_number);


static uint num_from_csname(CS_ID **cs, const char *name)
{
  CS_ID **c;
  for (c = cs; *c; ++c)
    if (!strcmp((*c)->name, name))
      return (*c)->number;
  return 0;   /* this mimics find_type() */
}

static char *name_from_csnum(CS_ID **cs, uint number)
{
  CS_ID **c;
  if(cs)
    for (c = cs; *c; ++c)
      if ((*c)->number == number)
	return (*c)->name;
  return (char*) "?";   /* this mimics find_type() */
}

static my_bool get_word(struct simpleconfig_buf_st *fb, char *buf)
{
  char *endptr=fb->p;

  for (;;)
  {
    while (isspace(*endptr))
      ++endptr;
    if (*endptr && *endptr != '#')		/* Not comment */
      break;					/* Found something */
    if ((fgets(fb->buf, sizeof(fb->buf), fb->f)) == NULL)
      return TRUE; /* end of file */
    endptr = fb->buf;
  }

  while (!isspace(*endptr))
    *buf++= *endptr++;
  *buf=0;
  fb->p = endptr;

  return FALSE;
}


char *get_charsets_dir(char *buf)
{
  const char *sharedir = SHAREDIR;
  DBUG_ENTER("get_charsets_dir");

  if (charsets_dir != NULL)
    strmake(buf, charsets_dir, FN_REFLEN-1);
  else
  {
    if (test_if_hard_path(sharedir) ||
	is_prefix(sharedir, DEFAULT_CHARSET_HOME))
      strxmov(buf, sharedir, "/", CHARSET_DIR, NullS);
    else
      strxmov(buf, DEFAULT_CHARSET_HOME, "/", sharedir, "/", CHARSET_DIR,
	      NullS);
  }
  convert_dirname(buf);
  DBUG_PRINT("info",("charsets dir='%s'", buf));
  DBUG_RETURN(strend(buf));
}


static my_bool read_charset_index(CS_ID ***charsets, myf myflags)
{
  struct simpleconfig_buf_st fb;
  char buf[MAX_LINE], num_buf[MAX_LINE];
  DYNAMIC_ARRAY cs;
  CS_ID *csid;

  strmov(get_charsets_dir(buf), "Index");

  if ((fb.f = my_fopen(buf, O_RDONLY, myflags)) == NULL)
    return TRUE;
  fb.buf[0] = '\0';
  fb.p = fb.buf;

  if (init_dynamic_array(&cs, sizeof(CS_ID *), 32, 32))
    return TRUE;

  while (!get_word(&fb, buf) && !get_word(&fb, num_buf))
  {
    uint csnum;
    uint length;

    if (!(csnum = atoi(num_buf)))
    {
      /* corrupt Index file */
      my_fclose(fb.f,myflags);
      return TRUE;
    }

    if (!(csid = (CS_ID*) my_once_alloc(sizeof(CS_ID), myflags)) ||
        !(csid->name=
           (char*) my_once_alloc(length= (uint) strlen(buf)+1, myflags)))
    {
      my_fclose(fb.f,myflags);
      return TRUE;
    }
    memcpy(csid->name,buf,length);
    csid->number = csnum;

    insert_dynamic(&cs, (gptr) &csid);
  }
  my_fclose(fb.f,myflags);


  if (!(*charsets =
      (CS_ID **) my_once_alloc((cs.elements + 1) * sizeof(CS_ID *), myflags)))
    return TRUE;
  /* unwarranted chumminess with dynamic_array implementation? */
  memcpy((byte *) *charsets, cs.buffer, cs.elements * sizeof(CS_ID *));
  (*charsets)[cs.elements] = NULL;
  delete_dynamic(&cs);  

  return FALSE;
}


static my_bool init_available_charsets(myf myflags)
{
  my_bool error=0;
  /*
    We have to use charset_initialized to not lock on THR_LOCK_charset
    inside get_internal_charset...
   */
  if (!charset_initialized)
  {
  /*
    To make things thread safe we are not allowing other threads to interfere
    while we may changing the cs_info_table
  */
    pthread_mutex_lock(&THR_LOCK_charset);
    if (!cs_info_table.buffer)			/* If not initialized */
    {
      init_dynamic_array(&cs_info_table, sizeof(CHARSET_INFO*), 16, 8);
      error = read_charset_index(&available_charsets, myflags);
    }
    charset_initialized=1;
    pthread_mutex_unlock(&THR_LOCK_charset);
  }
  if(!available_charsets || !available_charsets[0])
    error = TRUE;
  return error;
}


void free_charsets(void)
{
  delete_dynamic(&cs_info_table);
}


static my_bool fill_array(uchar *array, int sz, struct simpleconfig_buf_st *fb)
{
  char buf[MAX_LINE];
  while (sz--)
  {
    if (get_word(fb, buf))
    {
      DBUG_PRINT("error",("get_word failed, expecting %d more words", sz + 1));
      return 1;
    }
    *array++ = (uchar) strtol(buf, NULL, 16);
  }
  return 0;
}


static void get_charset_conf_name(uint cs_number, char *buf)
{
  strxmov(get_charsets_dir(buf),
          name_from_csnum(available_charsets, cs_number), ".conf", NullS);
}


static my_bool read_charset_file(uint cs_number, CHARSET_INFO *set,
				 myf myflags)
{
  struct simpleconfig_buf_st fb;
  char buf[FN_REFLEN];
  my_bool result;
  DBUG_ENTER("read_charset_file");
  DBUG_PRINT("enter",("cs_number: %d", cs_number));

  if (cs_number <= 0)
    DBUG_RETURN(TRUE);

  get_charset_conf_name(cs_number, buf);
  DBUG_PRINT("info",("file name: %s", buf));

  if ((fb.f = my_fopen(buf, O_RDONLY, myflags)) == NULL)
    DBUG_RETURN(TRUE);

  fb.buf[0] = '\0';				/* Init for get_word */
  fb.p = fb.buf;

  result=FALSE;
  if (fill_array(set->ctype,      CTYPE_TABLE_SIZE,      &fb) ||
      fill_array(set->to_lower,   TO_LOWER_TABLE_SIZE,   &fb) ||
      fill_array(set->to_upper,   TO_UPPER_TABLE_SIZE,   &fb) ||
      fill_array(set->sort_order, SORT_ORDER_TABLE_SIZE, &fb))
    result=TRUE;

  my_fclose(fb.f, MYF(0));
  DBUG_RETURN(result);
}


uint get_charset_number(const char *charset_name)
{
  my_bool error;
  error = init_available_charsets(MYF(0));	/* If it isn't initialized */
  if (error)
    return compiled_charset_number(charset_name);
  else
    return num_from_csname(available_charsets, charset_name);
}

const char *get_charset_name(uint charset_number)
{
  my_bool error;
  error = init_available_charsets(MYF(0));	/* If it isn't initialized */
  if (error)
    return compiled_charset_name(charset_number);
  else
    return name_from_csnum(available_charsets, charset_number);
}


static CHARSET_INFO *find_charset(CHARSET_INFO **table, uint cs_number,
                                  size_t tablesz)
{
  uint i;
  for (i = 0; i < tablesz; ++i)
    if (table[i]->number == cs_number)
      return table[i];
  return NULL;
}

static CHARSET_INFO *find_charset_by_name(CHARSET_INFO **table, const char *name,
					  size_t tablesz)
{
  uint i;
  for (i = 0; i < tablesz; ++i)
    if (!strcmp(table[i]->name,name))
      return table[i];
  return NULL;
}

static CHARSET_INFO *add_charset(uint cs_number, const char *cs_name)
{
  CHARSET_INFO tmp_cs,*cs;
  uchar tmp_ctype[CTYPE_TABLE_SIZE];
  uchar tmp_to_lower[TO_LOWER_TABLE_SIZE];
  uchar tmp_to_upper[TO_UPPER_TABLE_SIZE];
  uchar tmp_sort_order[SORT_ORDER_TABLE_SIZE];

  /* Don't allocate memory if we are not sure we can find the char set */
  cs= &tmp_cs;
  bzero((char*) cs, sizeof(*cs));
  cs->ctype=tmp_ctype;
  cs->to_lower=tmp_to_lower;
  cs->to_upper=tmp_to_upper;
  cs->sort_order=tmp_sort_order;
  if (read_charset_file(cs_number, cs, MYF(MY_WME)))
    return NULL;

  cs           = (CHARSET_INFO*) my_once_alloc(sizeof(CHARSET_INFO),
					       MYF(MY_WME));
  *cs=tmp_cs;
  cs->name     = (char *) my_once_alloc((uint) strlen(cs_name)+1, MYF(MY_WME));
  cs->ctype    = (uchar*) my_once_alloc(CTYPE_TABLE_SIZE,      MYF(MY_WME));
  cs->to_lower = (uchar*) my_once_alloc(TO_LOWER_TABLE_SIZE,   MYF(MY_WME));
  cs->to_upper = (uchar*) my_once_alloc(TO_UPPER_TABLE_SIZE,   MYF(MY_WME));
  cs->sort_order=(uchar*) my_once_alloc(SORT_ORDER_TABLE_SIZE, MYF(MY_WME));
  cs->number   = cs_number;
  memcpy((char*) cs->name,	 (char*) cs_name,	strlen(cs_name) + 1);
  memcpy((char*) cs->ctype,	 (char*) tmp_ctype,	sizeof(tmp_ctype));
  memcpy((char*) cs->to_lower, (char*) tmp_to_lower,	sizeof(tmp_to_lower));
  memcpy((char*) cs->to_upper, (char*) tmp_to_upper,	sizeof(tmp_to_upper));
  memcpy((char*) cs->sort_order, (char*) tmp_sort_order,
	 sizeof(tmp_sort_order));
  insert_dynamic(&cs_info_table, (gptr) &cs);
  return cs;
}

static CHARSET_INFO *get_internal_charset(uint cs_number)
{
  CHARSET_INFO *cs;
  /*
    To make things thread safe we are not allowing other threads to interfere
    while we may changing the cs_info_table
  */
  pthread_mutex_lock(&THR_LOCK_charset);
  if (!(cs = find_charset((CHARSET_INFO**) cs_info_table.buffer, cs_number,
			  cs_info_table.elements)))
    if (!(cs = find_compiled_charset(cs_number)))
      cs=add_charset(cs_number, get_charset_name(cs_number));
  pthread_mutex_unlock(&THR_LOCK_charset);
  return cs;
}


static CHARSET_INFO *get_internal_charset_by_name(const char *name)
{
  CHARSET_INFO *cs;
  /*
    To make things thread safe we are not allowing other threads to interfere
    while we may changing the cs_info_table
  */
  pthread_mutex_lock(&THR_LOCK_charset);
  if (!(cs = find_charset_by_name((CHARSET_INFO**) cs_info_table.buffer, name,
				 cs_info_table.elements)))
    if (!(cs = find_compiled_charset_by_name(name)))
      cs=add_charset(get_charset_number(name), name);
  pthread_mutex_unlock(&THR_LOCK_charset);
  return cs;
}


CHARSET_INFO *get_charset(uint cs_number, myf flags)
{
  CHARSET_INFO *cs;
  (void) init_available_charsets(MYF(0));	/* If it isn't initialized */
  cs=get_internal_charset(cs_number);

  if (!cs && (flags & MY_WME))
  {
    char index_file[FN_REFLEN], cs_string[23];
    strmov(get_charsets_dir(index_file), "Index");
    cs_string[0]='#';
    int10_to_str(cs_number, cs_string+1, 10);
    my_error(EE_UNKNOWN_CHARSET, MYF(ME_BELL), cs_string, index_file);
  }
  return cs;
}

my_bool set_default_charset(uint cs, myf flags)
{
  CHARSET_INFO *new_charset;
  DBUG_ENTER("set_default_charset");
  DBUG_PRINT("enter",("character set: %d",(int) cs));
  new_charset = get_charset(cs, flags);
  if (!new_charset)
  {
    DBUG_PRINT("error",("Couldn't set default character set"));
    DBUG_RETURN(TRUE);   /* error */
  }
  default_charset_info = new_charset;
  DBUG_RETURN(FALSE);
}

CHARSET_INFO *get_charset_by_name(const char *cs_name, myf flags)
{
  CHARSET_INFO *cs;
  (void) init_available_charsets(MYF(0));	/* If it isn't initialized */
  cs=get_internal_charset_by_name(cs_name);

  if (!cs && (flags & MY_WME))
  {
    char index_file[FN_REFLEN];
    strmov(get_charsets_dir(index_file), "Index");
    my_error(EE_UNKNOWN_CHARSET, MYF(ME_BELL), cs_name, index_file);
  }

  return cs;
}

my_bool set_default_charset_by_name(const char *cs_name, myf flags)
{
  CHARSET_INFO *new_charset;
  DBUG_ENTER("set_default_charset_by_name");
  DBUG_PRINT("enter",("character set: %s", cs_name));
  new_charset = get_charset_by_name(cs_name, flags);
  if (!new_charset)
  {
    DBUG_PRINT("error",("Couldn't set default character set"));
    DBUG_RETURN(TRUE);   /* error */
  }

  default_charset_info = new_charset;
  DBUG_RETURN(FALSE);
}

/* Only append name if it doesn't exist from before */

static my_bool charset_in_string(const char *name, DYNAMIC_STRING *s)
{
  uint length= (uint) strlen(name);
  const char *pos;
  for (pos=s->str ; (pos=strstr(pos,name)) ; pos++)
  {
    if (! pos[length] || pos[length] == ' ')
      return TRUE;				/* Already existed */
  }

  return FALSE;
}

static void charset_append(DYNAMIC_STRING *s, const char *name)
{
  if (!charset_in_string(name, s)) {
    dynstr_append(s, name);
    dynstr_append(s, " ");
  }
}


/* Returns a dynamically-allocated string listing the character sets
   requested.  The caller is responsible for freeing the memory. */

char * list_charsets(myf want_flags)
{
  DYNAMIC_STRING s;
  char *p;

  (void)init_available_charsets(MYF(0));
  init_dynamic_string(&s, NullS, 256, 1024);

  if (want_flags & MY_COMPILED_SETS)
  {
    CHARSET_INFO *cs;
    for (cs = compiled_charsets; cs->number > 0; cs++)
    {
      dynstr_append(&s, cs->name);
      dynstr_append(&s, " ");
    }
  }

  if (want_flags & MY_CONFIG_SETS)
  {
    CS_ID **c;
    char buf[FN_REFLEN];
    MY_STAT status;

    if((c=available_charsets))
      for (; *c; ++c)
	{
	  if (charset_in_string((*c)->name, &s))
	    continue;
	  get_charset_conf_name((*c)->number, buf);
	  if (!my_stat(buf, &status, MYF(0)))
	    continue;       /* conf file doesn't exist */
	  dynstr_append(&s, (*c)->name);
	  dynstr_append(&s, " ");
	}
  }

  if (want_flags & MY_INDEX_SETS)
  {
    CS_ID **c;
    for (c = available_charsets; *c; ++c)
      charset_append(&s, (*c)->name);
  }

  if (want_flags & MY_LOADED_SETS)
  {
    uint i;
    for (i = 0; i < cs_info_table.elements; i++)
      charset_append(&s, 
		     dynamic_element(&cs_info_table, i, CHARSET_INFO *)->name);
  }
  s.str[s.length - 1] = '\0';   /* chop trailing space */
  p = my_strdup(s.str, MYF(MY_WME));
  dynstr_free(&s);

  return p;
}

/****************************************************************************
* Code for debugging.
****************************************************************************/


static void _print_array(uint8 *data, uint size)
{
  uint i;
  for (i = 0; i < size; ++i)
  {
    if (i == 0 || i % 16 == size % 16) printf("  ");
    printf(" %02x", data[i]);
    if ((i+1) % 16 == size % 16) printf("\n");
  }
}

/* _print_csinfo is called from test_charset.c */
void _print_csinfo(CHARSET_INFO *cs)
{
  printf("%s #%d\n", cs->name, cs->number);
  printf("ctype:\n"); _print_array(cs->ctype, 257);
  printf("to_lower:\n"); _print_array(cs->to_lower, 256);
  printf("to_upper:\n"); _print_array(cs->to_upper, 256);
  printf("sort_order:\n"); _print_array(cs->sort_order, 256);
  printf("collate:    %3s (%d, %p, %p, %p, %p, %p)\n",
         cs->strxfrm_multiply ? "yes" : "no",
         cs->strxfrm_multiply,
         cs->strcoll,
         cs->strxfrm,
         cs->strnncoll,
         cs->strnxfrm,
         cs->like_range);
  printf("multi-byte: %3s (%d, %p, %p, %p)\n",
         cs->mbmaxlen ? "yes" : "no",
         cs->mbmaxlen,
         cs->ismbchar,
         cs->ismbhead,
         cs->mbcharlen);
}
