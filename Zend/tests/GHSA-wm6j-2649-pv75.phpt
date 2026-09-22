--TEST--
GHSA-wm6j-2649-pv75: Null pointer dereference in php_mb_check_encoding() via mb_ereg_search_init()
--CREDITS--
vi3tL0u1s
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (!function_exists('mb_regex_encoding')) die('skip No mbregex support');
?>
--FILE--
<?php
// iso-8859-11 is supported by Oniguruma but not by mbfl
mb_regex_encoding('iso-8859-11');
mb_ereg_search_init('x');
?>
--EXPECTF--
Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d

Fatal error: Uncaught ValueError: mb_regex_encoding(): Argument #1 ($encoding) must be a valid encoding, "iso-8859-11" given in %s:%d
Stack trace:
#0 %s(%d): mb_regex_encoding('iso-8859-11')
#1 {main}
  thrown in %s on line %d
