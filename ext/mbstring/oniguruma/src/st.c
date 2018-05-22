/* This is a public domain general purpose hash table package written by Peter Moore @ UCB. */

/* static	char	sccsid[] = "@(#) st.c 5.1 89/12/14 Crucible"; */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <malloc.h>
#endif

#include "regint.h"
#include "st.h"

typedef struct st_table_entry st_table_entry;

struct st_table_entry {
    unsigned int hash;
    st_data_t key;
    st_data_t record;
    st_table_entry *next;
};

#define ST_DEFAULT_MAX_DENSITY 5
#define ST_DEFAULT_INIT_TABLE_SIZE 11

    /*
     * DEFAULT_MAX_DENSITY is the default for the largest we allow the
     * average number of items per bin before increasing the number of
     * bins
     *
     * DEFAULT_INIT_TABLE_SIZE is the default for the number of bins
     * allocated initially
     *
     */

static int numcmp(long, long);
static int numhash(long);
static struct st_hash_type type_numhash = {
    numcmp,
    numhash,
};

/* extern int strcmp(const char *, const char *); */
static int strhash(const char *);
static struct st_hash_type type_strhash = {
    strcmp,
    strhash,
};

static void rehash(st_table *);

#define alloc(type) (type*)xmalloc((unsigned)sizeof(type))
#define Calloc(n,s) (char*)xcalloc((n),(s))

#define EQUAL(table,x,y) ((x)==(y) || (*table->type->compare)((x),(y)) == 0)

#define do_hash(key,table) (unsigned int)(*(table)->type->hash)((key))
#define do_hash_bin(key,table) (do_hash(key, table)%(table)->num_bins)

/*
 * MINSIZE is the minimum size of a dictionary.
 */

#define MINSIZE 8

/*
Table of prime numbers 2^n+a, 2<=n<=30.
*/
static const long primes[] = {
	8 + 3,
	16 + 3,
	32 + 5,
	64 + 3,
	128 + 3,
	256 + 27,
	512 + 9,
	1024 + 9,
	2048 + 5,
	4096 + 3,
	8192 + 27,
	16384 + 43,
	32768 + 3,
	65536 + 45,
	131072 + 29,
	262144 + 3,
	524288 + 21,
	1048576 + 7,
	2097152 + 17,
	4194304 + 15,
	8388608 + 9,
	16777216 + 43,
	33554432 + 35,
	67108864 + 15,
	134217728 + 29,
	268435456 + 3,
	536870912 + 11,
	1073741824 + 85,
	0
};

static int
new_size(size)
    int size;
{
    int i;

#if 0
    for (i=3; i<31; i++) {
      if ((1<<i) > size) return 1<<i;
    }
    return -1;
#else
    int newsize;

    for (i = 0, newsize = MINSIZE;
         i < (int )(sizeof(primes)/sizeof(primes[0]));
         i++, newsize <<= 1) {
      if (newsize > size) return primes[i];
    }
    /* Ran out of polynomials */
    return -1;			/* should raise exception */
#endif
}

#ifdef HASH_LOG
static int collision = 0;
static int init_st = 0;

static void
stat_col(void)
{
  FILE *f = fopen("/tmp/col", "w");
  if (f == 0) return ;

  (void) fprintf(f, "collision: %d\n", collision);
  (void) fclose(f);
}
#endif

st_table*
st_init_table_with_size(type, size)
    struct st_hash_type *type;
    int size;
{
  st_table *tbl;

#ifdef HASH_LOG
  if (init_st == 0) {
    init_st = 1;
    atexit(stat_col);
  }
#endif

  size = new_size(size);	/* round up to prime number */

  tbl = alloc(st_table);
  if (tbl == 0) return 0;

  tbl->type = type;
  tbl->num_entries = 0;
  tbl->num_bins = size;
  tbl->bins = (st_table_entry **)Calloc(size, sizeof(st_table_entry*));
  if (tbl->bins == 0) {
    free(tbl);
    return 0;
  }

  return tbl;
}

st_table*
st_init_table(type)
    struct st_hash_type *type;
{
  return st_init_table_with_size(type, 0);
}

st_table*
st_init_numtable(void)
{
  return st_init_table(&type_numhash);
}

st_table*
st_init_numtable_with_size(size)
    int size;
{
  return st_init_table_with_size(&type_numhash, size);
}

st_table*
st_init_strtable(void)
{
  return st_init_table(&type_strhash);
}

st_table*
st_init_strtable_with_size(size)
    int size;
{
  return st_init_table_with_size(&type_strhash, size);
}

void
st_free_table(table)
    st_table *table;
{
  register st_table_entry *ptr, *next;
  int i;

  for(i = 0; i < table->num_bins; i++) {
    ptr = table->bins[i];
    while (ptr != 0) {
	    next = ptr->next;
	    free(ptr);
	    ptr = next;
    }
  }
  free(table->bins);
  free(table);
}

#define PTR_NOT_EQUAL(table, ptr, hash_val, key) \
((ptr) != 0 && (ptr->hash != (hash_val) || !EQUAL((table), (key), (ptr)->key)))

#ifdef HASH_LOG
#define COLLISION collision++
#else
#define COLLISION
#endif

#define FIND_ENTRY(table, ptr, hash_val, bin_pos) do {\
    bin_pos = hash_val%(table)->num_bins;\
    ptr = (table)->bins[bin_pos];\
    if (PTR_NOT_EQUAL(table, ptr, hash_val, key)) {\
      COLLISION;\
      while (PTR_NOT_EQUAL(table, ptr->next, hash_val, key)) {\
        ptr = ptr->next;\
      }\
      ptr = ptr->next;\
    }\
} while (0)

int
st_lookup(table, key, value)
     st_table *table;
     register st_data_t key;
     st_data_t *value;
{
  unsigned int hash_val, bin_pos;
  register st_table_entry *ptr;

  hash_val = do_hash(key, table);
  FIND_ENTRY(table, ptr, hash_val, bin_pos);

  if (ptr == 0) {
    return 0;
  }
  else {
    if (value != 0)  *value = ptr->record;
    return 1;
  }
}

#define ADD_DIRECT(table, key, value, hash_val, bin_pos, ret) \
do {\
  st_table_entry *entry;\
  if (table->num_entries/(table->num_bins) > ST_DEFAULT_MAX_DENSITY) {\
    rehash(table);\
    bin_pos = hash_val % table->num_bins;\
  }\
  entry = alloc(st_table_entry);\
  if (IS_NULL(entry)) return ret;\
  entry->hash = hash_val;\
  entry->key = key;\
  entry->record = value;\
  entry->next = table->bins[bin_pos];\
  table->bins[bin_pos] = entry;\
  table->num_entries++;\
} while (0)

int
st_insert(table, key, value)
     register st_table *table;
     register st_data_t key;
     st_data_t value;
{
  unsigned int hash_val, bin_pos;
  register st_table_entry *ptr;

  hash_val = do_hash(key, table);
  FIND_ENTRY(table, ptr, hash_val, bin_pos);

  if (ptr == 0) {
    ADD_DIRECT(table, key, value, hash_val, bin_pos, ONIGERR_MEMORY);
    return 0;
  }
  else {
    ptr->record = value;
    return 1;
  }
}

void
st_add_direct(table, key, value)
     st_table *table;
     st_data_t key;
     st_data_t value;
{
  unsigned int hash_val, bin_pos;

  hash_val = do_hash(key, table);
  bin_pos = hash_val % table->num_bins;
  ADD_DIRECT(table, key, value, hash_val, bin_pos,);
}

static void
rehash(table)
     register st_table *table;
{
  register st_table_entry *ptr, *next, **new_bins;
  int i, old_num_bins = table->num_bins, new_num_bins;
  unsigned int hash_val;

  new_num_bins = new_size(old_num_bins+1);
  new_bins = (st_table_entry**)Calloc(new_num_bins, sizeof(st_table_entry*));
  if (new_bins == 0) {
    return ;
  }

  for(i = 0; i < old_num_bins; i++) {
    ptr = table->bins[i];
    while (ptr != 0) {
	    next = ptr->next;
	    hash_val = ptr->hash % new_num_bins;
	    ptr->next = new_bins[hash_val];
	    new_bins[hash_val] = ptr;
	    ptr = next;
    }
  }
  free(table->bins);
  table->num_bins = new_num_bins;
  table->bins = new_bins;
}

st_table*
st_copy(old_table)
     st_table *old_table;
{
  st_table *new_table;
  st_table_entry *ptr, *entry;
  int i, num_bins = old_table->num_bins;

  new_table = alloc(st_table);
  if (new_table == 0) {
    return 0;
  }

  *new_table = *old_table;
  new_table->bins = (st_table_entry**)
    Calloc((unsigned)num_bins, sizeof(st_table_entry*));

  if (new_table->bins == 0) {
    free(new_table);
    return 0;
  }

  for(i = 0; i < num_bins; i++) {
    new_table->bins[i] = 0;
    ptr = old_table->bins[i];
    while (ptr != 0) {
	    entry = alloc(st_table_entry);
	    if (entry == 0) {
        free(new_table->bins);
        free(new_table);
        return 0;
	    }
	    *entry = *ptr;
	    entry->next = new_table->bins[i];
	    new_table->bins[i] = entry;
	    ptr = ptr->next;
    }
  }
  return new_table;
}

int
st_delete(table, key, value)
     register st_table *table;
     register st_data_t *key;
     st_data_t *value;
{
  unsigned int hash_val;
  st_table_entry *tmp;
  register st_table_entry *ptr;

  hash_val = do_hash_bin(*key, table);
  ptr = table->bins[hash_val];

  if (ptr == 0) {
    if (value != 0) *value = 0;
    return 0;
  }

  if (EQUAL(table, *key, ptr->key)) {
    table->bins[hash_val] = ptr->next;
    table->num_entries--;
    if (value != 0) *value = ptr->record;
    *key = ptr->key;
    free(ptr);
    return 1;
  }

  for(; ptr->next != 0; ptr = ptr->next) {
    if (EQUAL(table, ptr->next->key, *key)) {
	    tmp = ptr->next;
	    ptr->next = ptr->next->next;
	    table->num_entries--;
	    if (value != 0) *value = tmp->record;
	    *key = tmp->key;
	    free(tmp);
	    return 1;
    }
  }

  return 0;
}

int
st_delete_safe(table, key, value, never)
     register st_table *table;
     register st_data_t *key;
     st_data_t *value;
     st_data_t never;
{
  unsigned int hash_val;
  register st_table_entry *ptr;

  hash_val = do_hash_bin(*key, table);
  ptr = table->bins[hash_val];

  if (ptr == 0) {
    if (value != 0) *value = 0;
    return 0;
  }

  for(; ptr != 0; ptr = ptr->next) {
    if ((ptr->key != never) && EQUAL(table, ptr->key, *key)) {
	    table->num_entries--;
	    *key = ptr->key;
	    if (value != 0) *value = ptr->record;
	    ptr->key = ptr->record = never;
	    return 1;
    }
  }

  return 0;
}

static int
#if defined(__GNUC__)
delete_never(st_data_t key __attribute__ ((unused)), st_data_t value,
	     st_data_t never)
#else
delete_never(key, value, never)
    st_data_t key, value, never;
#endif
{
    if (value == never) return ST_DELETE;
    return ST_CONTINUE;
}

void
st_cleanup_safe(table, never)
     st_table *table;
     st_data_t never;
{
  int num_entries = table->num_entries;

  st_foreach(table, delete_never, never);
  table->num_entries = num_entries;
}

int
st_foreach(table, func, arg)
     st_table *table;
     int (*func)();
     st_data_t arg;
{
  st_table_entry *ptr, *last, *tmp;
  enum st_retval retval;
  int i;

  for(i = 0; i < table->num_bins; i++) {
    last = 0;
    for(ptr = table->bins[i]; ptr != 0;) {
	    retval = (*func)(ptr->key, ptr->record, arg);
	    switch (retval) {
	    case ST_CHECK:	/* check if hash is modified during iteration */
        tmp = 0;
        if (i < table->num_bins) {
          for (tmp = table->bins[i]; tmp; tmp=tmp->next) {
            if (tmp == ptr) break;
          }
        }
        if (!tmp) {
          /* call func with error notice */
          return 1;
        }
        /* fall through */
	    case ST_CONTINUE:
        last = ptr;
        ptr = ptr->next;
        break;
	    case ST_STOP:
        return 0;
	    case ST_DELETE:
        tmp = ptr;
        if (last == 0) {
          table->bins[i] = ptr->next;
        }
        else {
          last->next = ptr->next;
        }
        ptr = ptr->next;
        free(tmp);
        table->num_entries--;
	    }
    }
  }
  return 0;
}

static int
strhash(string)
     register const char *string;
{
  register int c;

#ifdef HASH_ELFHASH
  register unsigned int h = 0, g;

  while ((c = *string++) != '\0') {
    h = ( h << 4 ) + c;
    if ( g = h & 0xF0000000 )
	    h ^= g >> 24;
    h &= ~g;
  }
  return h;
#elif HASH_PERL
  register int val = 0;

  while ((c = *string++) != '\0') {
    val += c;
    val += (val << 10);
    val ^= (val >> 6);
  }
  val += (val << 3);
  val ^= (val >> 11);

  return val + (val << 15);
#else
  register int val = 0;

  while ((c = *string++) != '\0') {
    val = val*997 + c;
  }

  return val + (val>>5);
#endif
}

static int
numcmp(x, y)
     long x, y;
{
  return x != y;
}

static int
numhash(n)
     long n;
{
  return n;
}
