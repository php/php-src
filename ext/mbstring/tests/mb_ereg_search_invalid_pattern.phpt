--TEST--
mb_ereg_search() with invalid pattern should discard old matches
--EXTENSIONS--
mbstring
--SKIPIF--
<?php if (!function_exists("mb_ereg")) print "skip requires mbregex"; ?>
--FILE--
<?php

mb_ereg_search_init('');
var_dump(mb_ereg_search(''));
var_dump(mb_ereg_search("\xff"));
var_dump(mb_ereg_search_getregs());

?>
--EXPECTF--
Deprecated: Function mb_ereg_search_init() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d

Deprecated: Function mb_ereg_search() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
bool(true)

Deprecated: Function mb_ereg_search() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d

Warning: mb_ereg_search(): Pattern is not valid under UTF-8 encoding in %s on line %d
bool(false)

Deprecated: Function mb_ereg_search_getregs() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
bool(false)
