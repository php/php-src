--TEST--
Bug #73646 (mb_ereg_search_init null pointer dereference)
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (!function_exists('mb_ereg')) die('skip mbregex support not available');
?>
--FILE--
<?php
var_dump(mb_ereg_search_init(NULL));
?>
--EXPECTF--
Deprecated: Function mb_ereg_search_init() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d

Deprecated: mb_ereg_search_init(): Passing null to parameter #1 ($string) of type string is deprecated in %s on line %d
bool(true)
