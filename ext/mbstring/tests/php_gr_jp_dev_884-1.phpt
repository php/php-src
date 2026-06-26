--TEST--
php-dev@php.gr.jp #884 (1)
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
function_exists('mb_ereg_replace') or die("skip mb_ereg_replace() is not available in this build");
?>
--FILE--
<?php
set_time_limit(2);
var_dump(preg_replace("/.*/", "b", "a"));
var_dump(mb_ereg_replace(".*", "b", "a"));
?>
--EXPECTF--
string(2) "bb"

Deprecated: Function mb_ereg_replace() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(2) "bb"
