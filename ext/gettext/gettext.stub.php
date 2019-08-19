<?php

/** @return string|false */
function textdomain(?string $domain) {}

/** @return string|false */
function gettext(string $msgid) {}

/** @return string|false */
function dgettext(string $domain_name, string $msgid) {}

/** @return string|false */
function dcgettext(string $domain_name, string $msgid, int $category) {}

/** @return string|false */
function bindtextdomain(string $domain_name, string $dir) {}

#if HAVE_NGETTEXT
/** @return string|false */
function ngettext(string $msgid1, string $msgid2, int $n) {}
#endif

#if HAVE_DNGETTEXT
/** @return string|false */
function dngettext(string $domain, string $msgid1, string $msgid2, int $count) {}
#endif

#if HAVE_DCNGETTEXT
/** @return string|false */
function dcngettext(string $domain, string $msgid1, string $msgid2, int $count, int $category) {}
#endif

#if HAVE_BIND_TEXTDOMAIN_CODESET
/** @return string|false */
function bind_textdomain_codeset(string $domain, string $codeset) {}
#endif
