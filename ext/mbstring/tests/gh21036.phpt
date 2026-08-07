--TEST--
GH-21036 (mb_ereg_search_getregs() after regex cache invalidation)
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (!function_exists("mb_ereg")) print "skip mb_ereg() not available";
?>
--FILE--
<?php
error_reporting(E_ALL & ~E_DEPRECATED);

$pattern = '(?<name>a)';
mb_ereg_search_init('a', $pattern);
mb_ereg_search_pos();
mb_eregi($pattern, 'a');

var_dump(mb_ereg_search_getregs());
?>
--EXPECT--
bool(false)
