--TEST--
Bug #77367 (Negative size parameter in mb_split)
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (!function_exists('mb_split')) die('skip mb_split() not available');
?>
--FILE--
<?php
mb_regex_encoding('UTF-8');
var_dump(mb_split("\\w", "\xfc"));
?>
--EXPECTF--
Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d

Deprecated: Function mb_split() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
bool(false)
