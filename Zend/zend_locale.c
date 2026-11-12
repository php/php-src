/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright © Zend Technologies Ltd., a subsidiary company of          |
   |     Perforce Software, Inc., and Contributors.                       |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Felix Bünemann <felix.buenemann@gmail.com>                |
   +----------------------------------------------------------------------+
*/

#include "zend_locale.h"

#ifdef ZEND_THREAD_LOCALE

#include "zend.h"
#include "zend_globals.h"
#include "zend_smart_str.h"

#ifdef HAVE_XLOCALE_H
# include <xlocale.h>
#endif

/* setlocale() changes the locale of the whole process and isn't thread-safe.
 * zend_setlocale() switches the current thread to a locale of its own with
 * uselocale() instead. The locale dependent functions of the C library then
 * use that locale in place of the process locale. Builds that aren't
 * thread-safe work the same way, with a single thread. */

#if defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
# define ZEND_LOCALE_BSD 1
#elif defined(__MUSL__)
# define ZEND_LOCALE_MUSL 1
#endif

/* Indexed by category */
static const struct {
	int mask;
	const char *name;
} categories[] = {
	[LC_ALL] = {LC_ALL_MASK, "LC_ALL"},
	[LC_COLLATE] = {LC_COLLATE_MASK, "LC_COLLATE"},
	[LC_CTYPE] = {LC_CTYPE_MASK, "LC_CTYPE"},
	[LC_MONETARY] = {LC_MONETARY_MASK, "LC_MONETARY"},
	[LC_NUMERIC] = {LC_NUMERIC_MASK, "LC_NUMERIC"},
	[LC_TIME] = {LC_TIME_MASK, "LC_TIME"},
	[LC_MESSAGES] = {LC_MESSAGES_MASK, "LC_MESSAGES"},
#ifdef LC_PAPER_MASK
	[LC_PAPER] = {LC_PAPER_MASK, "LC_PAPER"},
#endif
#ifdef LC_NAME_MASK
	[LC_NAME] = {LC_NAME_MASK, "LC_NAME"},
#endif
#ifdef LC_ADDRESS_MASK
	[LC_ADDRESS] = {LC_ADDRESS_MASK, "LC_ADDRESS"},
#endif
#ifdef LC_TELEPHONE_MASK
	[LC_TELEPHONE] = {LC_TELEPHONE_MASK, "LC_TELEPHONE"},
#endif
#ifdef LC_MEASUREMENT_MASK
	[LC_MEASUREMENT] = {LC_MEASUREMENT_MASK, "LC_MEASUREMENT"},
#endif
#ifdef LC_IDENTIFICATION_MASK
	[LC_IDENTIFICATION] = {LC_IDENTIFICATION_MASK, "LC_IDENTIFICATION"},
#endif
};

#define CATEGORY_COUNT ((int) (sizeof(categories) / sizeof(categories[0])))

/* All categories except LC_ALL, in the order of their constants. The C library
 * uses the same order for the names in the name of LC_ALL. */
#define FOREACH_CATEGORY(category) \
	for (int category = 0; category < CATEGORY_COUNT; category++) \
		if (category != LC_ALL && categories[category].name)

/* When the categories have different locales, the name of LC_ALL is made of
 * their names, as in the C library: "C/C.UTF-8/C/C/C/C" on macOS and BSD,
 * "LC_CTYPE=C.UTF-8;LC_NUMERIC=C;..." on glibc and "C.UTF-8;C;C;C;C;C"
 * elsewhere (musl). */
#ifdef ZEND_LOCALE_BSD
# define NAME_SEPARATOR "/"
#else
# define NAME_SEPARATOR ";"
#endif

typedef struct _zend_locale {
	locale_t locale;
	struct _zend_locale *next;
	/* Indexed by category */
	char *names[CATEGORY_COUNT];
} zend_locale;

/* Shared by all threads and never freed: threads keep using them, and a
 * process only needs one for each combination of locales. A locale doesn't
 * change once it is in the list and new ones become the head, so the list can
 * be read without a lock. */
static _Atomic(zend_locale *) locales = NULL;

/* Locale of the current thread, NULL for the process locale */
ZEND_TLS zend_locale *thread_locale = NULL;

/* The name setlocale() returns for a locale. The name "" is the locale of the
 * environment and needs the environment lock. */
static const char *zend_locale_resolve_name(int category, const char *name)
{
	if (name[0] == '\0') {
		/* The locale of the environment */
		const char *variables[] = {"LC_ALL", categories[category].name, "LANG"};

#ifdef ZEND_LOCALE_MUSL
		name = "C.UTF-8";
#else
		name = "C";
#endif
		for (size_t i = 0; i < sizeof(variables) / sizeof(variables[0]); i++) {
			const char *value = getenv(variables[i]);
			if (value && value[0] != '\0') {
				name = value;
				break;
			}
		}
	}

#ifndef ZEND_LOCALE_BSD
	if (strcmp(name, "POSIX") == 0) {
		return "C";
	}
#endif
#ifdef ZEND_LOCALE_MUSL
	/* musl only has a C.UTF-8 locale for LC_CTYPE */
	if (category != LC_CTYPE && strcmp(name, "C.UTF-8") == 0) {
		return "C";
	}
#endif

	return name;
}

/* Splits a name of LC_ALL into the names of the categories */
static bool zend_locale_split_name(char *all, const char **names)
{
#ifdef __GLIBC__
	int count = 0;
	char *last = NULL;

	for (char *name = strtok_r(all, NAME_SEPARATOR, &last); name; name = strtok_r(NULL, NAME_SEPARATOR, &last)) {
		char *value = strchr(name, '=');
		if (!value) {
			return false;
		}
		*value = '\0';

		int category = 0;
		while (category < CATEGORY_COUNT && (category == LC_ALL || strcmp(categories[category].name, name) != 0)) {
			category++;
		}
		if (category == CATEGORY_COUNT) {
			return false;
		}
		names[category] = value + 1;
		count++;
	}

	return count == CATEGORY_COUNT - 1;
#else
	FOREACH_CATEGORY(category) {
		char *end = strstr(all, NAME_SEPARATOR);
		if (end) {
			*end = '\0';
		}
#ifdef ZEND_LOCALE_BSD
		/* Elsewhere, an empty name is the locale of the environment */
		if (all[0] == '\0') {
			return false;
		}
#endif
		names[category] = all;
		/* The last name applies to the remaining categories */
		if (end) {
			all = end + 1;
		}
	}

	return true;
#endif
}

/* The name of LC_ALL for the names of the categories */
static char *zend_locale_join_names(const char **names)
{
	smart_str all = {0};
	bool equal = true;

	FOREACH_CATEGORY(category) {
		if (all.s) {
			smart_str_appends_ex(&all, NAME_SEPARATOR, true);
		}
#ifdef __GLIBC__
		smart_str_appends_ex(&all, categories[category].name, true);
		smart_str_appendc_ex(&all, '=', true);
#endif
		smart_str_appends_ex(&all, names[category], true);
		equal = equal && strcmp(names[category], names[LC_CTYPE]) == 0;
	}
	smart_str_0(&all);

	char *name = pestrdup(equal ? names[LC_CTYPE] : ZSTR_VAL(all.s), 1);
	smart_str_free_ex(&all, true);

	return name;
}

static zend_locale *zend_locale_find(const char **names)
{
	for (zend_locale *locale = atomic_load_explicit(&locales, memory_order_acquire); locale; locale = locale->next) {
		bool equal = true;

		FOREACH_CATEGORY(category) {
			equal = equal && strcmp(locale->names[category], names[category]) == 0;
		}
		if (equal) {
			return locale;
		}
	}

	return NULL;
}

/* Returns the shared locale with these names, NULL if it isn't available */
static zend_locale *zend_locale_get(const char **names)
{
	bool environment = false;

	FOREACH_CATEGORY(category) {
		environment = environment || names[category][0] == '\0';
	}
	if (!environment) {
		FOREACH_CATEGORY(category) {
			names[category] = zend_locale_resolve_name(category, names[category]);
		}

		zend_locale *found = zend_locale_find(names);
		if (found) {
			return found;
		}
	}

	/* Locales are created one at a time: concurrent newlocale() calls crash
	 * on macOS, and the "" locale reads the environment */
	tsrm_env_lock();

	FOREACH_CATEGORY(category) {
		names[category] = zend_locale_resolve_name(category, names[category]);
	}

	zend_locale *found = zend_locale_find(names);
	if (!found) {
		locale_t locale = newlocale(LC_ALL_MASK, "C", (locale_t) 0);

		FOREACH_CATEGORY(category) {
			if (locale) {
				locale_t base = locale;
				locale = newlocale(categories[category].mask, names[category], base);
				/* On success, newlocale() takes over the base locale, except
				 * on macOS */
#ifdef __APPLE__
				freelocale(base);
#else
				if (!locale) {
					freelocale(base);
				}
#endif
			}
		}

		if (locale) {
			found = pemalloc(sizeof(zend_locale), 1);
			found->locale = locale;
			FOREACH_CATEGORY(category) {
				found->names[category] = pestrdup(names[category], 1);
			}
			found->names[LC_ALL] = zend_locale_join_names(names);
			found->next = atomic_load_explicit(&locales, memory_order_relaxed);
			atomic_store_explicit(&locales, found, memory_order_release);
		}
	}

	tsrm_env_unlock();

	return found;
}

ZEND_API const char *zend_setlocale(int category, const char *locale)
{
	/* Outside of a request, the process locale is changed: it's set during
	 * startup and is the initial locale of all threads */
	if (!EG(active)) {
		if (locale && thread_locale) {
			uselocale(LC_GLOBAL_LOCALE);
			thread_locale = NULL;
		}
		return setlocale(category, locale);
	}

	if (category < 0 || category >= CATEGORY_COUNT || !categories[category].name) {
		return NULL;
	}

	const char *names[CATEGORY_COUNT];
	zend_locale *current = thread_locale;

	if (!current) {
		/* The thread still uses the process locale. Its name of LC_ALL comes
		 * from a shared locale, as querying it isn't thread-safe. */
		FOREACH_CATEGORY(i) {
			names[i] = setlocale(i, NULL);
		}
		current = zend_locale_get(names);
	}
	if (!locale || !current) {
		return current ? current->names[category] : NULL;
	}

	char *all = NULL;

	FOREACH_CATEGORY(i) {
		names[i] = (category == LC_ALL || category == i) ? locale : current->names[i];
	}
	if (category == LC_ALL && strstr(locale, NAME_SEPARATOR)) {
		all = estrdup(locale);
		if (!zend_locale_split_name(all, names)) {
			efree(all);
			return NULL;
		}
	}

	zend_locale *found = zend_locale_get(names);
	if (all) {
		efree(all);
	}
	if (!found) {
		return NULL;
	}

	uselocale(found->locale);
	thread_locale = found;

	return found->names[category];
}

#endif
