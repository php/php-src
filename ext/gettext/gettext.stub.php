<?php

/** @generate-function-entries */

function textdomain(?string $domain): string {}

function gettext(string $msgid): string {}

/** @alias gettext */
function _(string $msgid): string {}

function dgettext(string $domain, string $msgid): string {}

function dcgettext(string $domain, string $msgid, int $category): string {}

function bindtextdomain(string $domain, string $dir): string|false {}

#ifdef HAVE_NGETTEXT
function ngettext(string $msgid1, string $msgid2, int $n): string {}
#endif

#ifdef HAVE_DNGETTEXT
function dngettext(string $domain, string $msgid1, string $msgid2, int $count): string {}
#endif

#ifdef HAVE_DCNGETTEXT
function dcngettext(string $domain, string $msgid1, string $msgid2, int $count, int $category): string {}
#endif

#ifdef HAVE_BIND_TEXTDOMAIN_CODESET
function bind_textdomain_codeset(string $domain, string $codeset): string|false {}
#endif
