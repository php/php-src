--TEST--
mb_ereg_search() with invalid pattern should discard old matches
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
bool(true)

Warning: mb_ereg_search(): Pattern is not valid under UTF-8 encoding in %s on line %d
bool(false)
bool(false)
