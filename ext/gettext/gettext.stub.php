<?php

/** @generate-function-entries */

function textdomain(?string $domain): string {}

function gettext(string $message): string {}

/** @alias gettext */
function _(string $message): string {}

function dgettext(string $domain, string $message): string {}

function dcgettext(string $domain, string $message, int $category): string {}

function bindtextdomain(string $domain, ?string $directory): string|false {}

#ifdef HAVE_NGETTEXT
function ngettext(string $singular, string $plural, int $count): string {}
#endif

#ifdef HAVE_DNGETTEXT
function dngettext(string $domain, string $singular, string $plural, int $count): string {}
#endif

#ifdef HAVE_DCNGETTEXT
function dcngettext(string $domain, string $singular, string $plural, int $count, int $category): string {}
#endif

#ifdef HAVE_BIND_TEXTDOMAIN_CODESET
function bind_textdomain_codeset(string $domain, ?string $codeset): string|false {}
#endif
