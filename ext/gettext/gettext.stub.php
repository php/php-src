<?php

/** @generate-class-entries */

/** @refcount 1 */
function textdomain(?string $domain): string {}

/** @refcount 1 */
function gettext(string $message): string {}

/** @alias gettext */
function _(string $message): string {}

/** @refcount 1 */
function dgettext(string $domain, string $message): string {}

/** @refcount 1 */
function dcgettext(string $domain, string $message, int $category): string {}

/** @refcount 1 */
function bindtextdomain(string $domain, ?string $directory): string|false {}

#ifdef HAVE_NGETTEXT
/** @refcount 1 */
function ngettext(string $singular, string $plural, int $count): string {}
#endif

#ifdef HAVE_DNGETTEXT
/** @refcount 1 */
function dngettext(string $domain, string $singular, string $plural, int $count): string {}
#endif

#ifdef HAVE_DCNGETTEXT
/** @refcount 1 */
function dcngettext(string $domain, string $singular, string $plural, int $count, int $category): string {}
#endif

#ifdef HAVE_BIND_TEXTDOMAIN_CODESET
/** @refcount 1 */
function bind_textdomain_codeset(string $domain, ?string $codeset): string|false {}
#endif
