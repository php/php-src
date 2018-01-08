/*
  zip_hash.c -- hash table string -> uint64
  Copyright (c) 2015-2017 Dieter Baron and Thomas Klausner

  This file is part of libzip, a library to manipulate ZIP archives.
  The authors can be contacted at <libzip@nih.at>

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:
  1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
  3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

  THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
  IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
  IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdlib.h>
#include <string.h>
#include "zipint.h"

struct zip_hash_entry {
    const zip_uint8_t *name;
    zip_int64_t orig_index;
    zip_int64_t current_index;
    struct zip_hash_entry *next;
};
typedef struct zip_hash_entry zip_hash_entry_t;

struct zip_hash {
    zip_uint16_t table_size;
    zip_hash_entry_t **table;
};

zip_hash_t *
_zip_hash_new(zip_uint16_t table_size, zip_error_t *error)
{
    zip_hash_t *hash;

    if (table_size == 0) {
	zip_error_set(error, ZIP_ER_INTERNAL, 0);
	return NULL;
    }

    if ((hash=(zip_hash_t *)malloc(sizeof(zip_hash_t))) == NULL) {
	zip_error_set(error, ZIP_ER_MEMORY, 0);
	return NULL;
    }
    hash->table_size = table_size;
    if ((hash->table=(zip_hash_entry_t**)calloc(table_size, sizeof(zip_hash_entry_t *))) == NULL) {
	free(hash);
	zip_error_set(error, ZIP_ER_MEMORY, 0);
	return NULL;
    }

    return hash;
}

static void
_free_list(zip_hash_entry_t *entry)
{
    zip_hash_entry_t *next;
    do {
	next = entry->next;
	free(entry);
	entry = next;
    } while (entry != NULL);
}

void
_zip_hash_free(zip_hash_t *hash)
{
    zip_uint16_t i;

    if (hash == NULL) {
	return;
    }

    for (i=0; i<hash->table_size; i++) {
	if (hash->table[i] != NULL) {
	    _free_list(hash->table[i]);
	}
    }
    free(hash->table);
    free(hash);
}

static zip_uint16_t
_hash_string(const zip_uint8_t *name, zip_uint16_t size)
{
#define HASH_MULTIPLIER 33
    zip_uint16_t value = 5381;

    if (name == NULL)
	return 0;

    while (*name != 0) {
	value = (zip_uint16_t)(((value * HASH_MULTIPLIER) + (zip_uint8_t)*name) % size);
	name++;
    }

    return value;
}

/* insert into hash, return error on existence or memory issues */
bool
_zip_hash_add(zip_hash_t *hash, const zip_uint8_t *name, zip_uint64_t index, zip_flags_t flags, zip_error_t *error)
{
    zip_uint16_t hash_value;
    zip_hash_entry_t *entry;

    if (hash == NULL || name == NULL || index > ZIP_INT64_MAX) {
	zip_error_set(error, ZIP_ER_INVAL, 0);
	return false;
    }

    hash_value = _hash_string(name, hash->table_size);
    for (entry = hash->table[hash_value]; entry != NULL; entry = entry->next) {
	if (strcmp((const char *)name, (const char *)entry->name) == 0) {
	    if (((flags & ZIP_FL_UNCHANGED) && entry->orig_index != -1) || entry->current_index != -1) {
		zip_error_set(error, ZIP_ER_EXISTS, 0);
		return false;
	    }
	    else {
		break;
	    }
	}
    }

    if (entry == NULL) {
	if ((entry=(zip_hash_entry_t *)malloc(sizeof(zip_hash_entry_t))) == NULL) {
	    zip_error_set(error, ZIP_ER_MEMORY, 0);
	    return false;
	}
	entry->name = name;
	entry->next = hash->table[hash_value];
	hash->table[hash_value] = entry;
	entry->orig_index = -1;
    }

    if (flags & ZIP_FL_UNCHANGED) {
	entry->orig_index = (zip_int64_t)index;
    }
    entry->current_index = (zip_int64_t)index;

    return true;
}

/* remove entry from hash, error if not found */
bool
_zip_hash_delete(zip_hash_t *hash, const zip_uint8_t *name, zip_error_t *error)
{
    zip_uint16_t hash_value;
    zip_hash_entry_t *entry, *previous;

    if (hash == NULL || name == NULL) {
	zip_error_set(error, ZIP_ER_INVAL, 0);
	return false;
    }

    hash_value = _hash_string(name, hash->table_size);
    previous = NULL;
    entry = hash->table[hash_value];
    while (entry) {
	if (strcmp((const char *)name, (const char *)entry->name) == 0) {
	    if (entry->orig_index == -1) {
		if (previous) {
		    previous->next = entry->next;
		}
		else {
		    hash->table[hash_value] = entry->next;
		}
		free(entry);
	    }
	    else {
		entry->current_index = -1;
	    }
	    return true;
	}
	previous = entry;
	entry = entry->next;
    };

    zip_error_set(error, ZIP_ER_NOENT, 0);
    return false;
}

/* find value for entry in hash, -1 if not found */
zip_int64_t
_zip_hash_lookup(zip_hash_t *hash, const zip_uint8_t *name, zip_flags_t flags, zip_error_t *error)
{
    zip_uint16_t hash_value;
    zip_hash_entry_t *entry;

    if (hash == NULL || name == NULL) {
	zip_error_set(error, ZIP_ER_INVAL, 0);
	return -1;
    }

    hash_value = _hash_string(name, hash->table_size);
    for (entry = hash->table[hash_value]; entry != NULL; entry = entry->next) {
	if (strcmp((const char *)name, (const char *)entry->name) == 0) {
	    if (flags & ZIP_FL_UNCHANGED) {
		if (entry->orig_index != -1) {
		    return entry->orig_index;
		}
	    }
	    else {
		if (entry->current_index != -1) {
		    return entry->current_index;
		}
	    }
	    break;
	}
    }

    zip_error_set(error, ZIP_ER_NOENT, 0);
    return -1;
}

void
_zip_hash_revert(zip_hash_t *hash)
{
    zip_uint16_t i;
    zip_hash_entry_t *entry, *previous;

    for (i = 0; i < hash->table_size; i++) {
	previous = NULL;
	entry = hash->table[i];
	while (entry) {
	    if (entry->orig_index == -1) {
		zip_hash_entry_t *p;
		if (previous) {
		    previous->next = entry->next;
		}
		else {
		    hash->table[i] = entry->next;
		}
		p = entry;
		entry = entry->next;
		/* previous does not change */
		free(p);
	    }
	    else {
		entry->current_index = entry->orig_index;
		previous = entry;
		entry = entry->next;
	    }
	}
    }
}
