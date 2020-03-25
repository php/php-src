--TEST--
Bug #72691 (mb_ereg_search raises a warning if a match zero-width)
--SKIPIF--
<?php
if (!extension_loaded('mbstring')) die('skip ext/mbstring required');
if (!function_exists('mb_ereg')) die('skip mbregex support not available');
?>
--FILE--
<?php
$str = 'foo';
mb_ereg_search_init($str);

mb_ereg_search('\A');
var_dump(mb_ereg_search_getpos());
var_dump(mb_ereg_search_getregs());

mb_ereg_search('\s*');
var_dump(mb_ereg_search_getpos());
var_dump(mb_ereg_search_getregs());

mb_ereg_search('\w+');
var_dump(mb_ereg_search_getpos());
var_dump(mb_ereg_search_getregs());

mb_ereg_search('\Z');
var_dump(mb_ereg_search_getpos());
var_dump(mb_ereg_search_getregs());
?>
--EXPECT--
int(0)
array(1) {
  [0]=>
  string(0) ""
}
int(0)
array(1) {
  [0]=>
  string(0) ""
}
int(3)
array(1) {
  [0]=>
  string(3) "foo"
}
int(3)
array(1) {
  [0]=>
  string(0) ""
}
