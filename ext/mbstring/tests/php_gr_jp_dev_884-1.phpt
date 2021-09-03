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
--EXPECT--
string(2) "bb"
string(2) "bb"
