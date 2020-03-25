<?php

function textdomain(?string $domain): string|false {}

function gettext(string $msgid): string|false {}

function dgettext(string $domain_name, string $msgid): string|false {}

function dcgettext(string $domain_name, string $msgid, int $category): string|false {}

function bindtextdomain(string $domain_name, string $dir): string|false {}

#if HAVE_NGETTEXT
function ngettext(string $msgid1, string $msgid2, int $n): string|false {}
#endif

#if HAVE_DNGETTEXT
function dngettext(string $domain, string $msgid1, string $msgid2, int $count): string|false {}
#endif

#if HAVE_DCNGETTEXT
function dcngettext(string $domain, string $msgid1, string $msgid2, int $count, int $category): string|false {}
#endif

#if HAVE_BIND_TEXTDOMAIN_CODESET
function bind_textdomain_codeset(string $domain, string $codeset): string|false {}
#endif
