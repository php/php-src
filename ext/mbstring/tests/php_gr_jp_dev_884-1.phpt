--TEST--
php-dev@php.gr.jp #884 (1)
--FILE--
<?php
set_time_limit(2);
var_dump(ereg_replace(".*", "b", "a"));
var_dump(preg_replace("/.*/", "b", "a"));
var_dump(mb_ereg_replace(".*", "b", "a"));
?>
--EXPECT--
string(2) "bb"
string(2) "bb"
string(2) "bb"
