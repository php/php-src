--TEST--
Bug #72693 (mb_ereg_search increments search position when a match zero-width)
--SKIPIF--
<?php
if (!extension_loaded('mbstring')) die('skip ext/mbstring required');
?>
--FILE--
<?php
mb_ereg_search_init('foo');

var_dump(mb_ereg_search('\A'));
var_dump(mb_ereg_search_getpos());

var_dump(mb_ereg_search('\s*'));
var_dump(mb_ereg_search_getpos());

var_dump(mb_ereg_search('\w+'));
var_dump(mb_ereg_search_getpos());
var_dump(mb_ereg_search_getregs());

var_dump(mb_ereg_search('\s*'));
var_dump(mb_ereg_search_getpos());

var_dump(mb_ereg_search('\Z'));
var_dump(mb_ereg_search_getpos());
?>
--EXPECT--
bool(true)
int(0)
bool(true)
int(0)
bool(true)
int(3)
array(1) {
  [0]=>
  string(3) "foo"
}
bool(true)
int(3)
bool(true)
int(3)
