--TEST--
Bug #72694 (mb_ereg_search_setpos does not accept a string's last position)
--SKIPIF--
<?php
if (!extension_loaded('mbstring')) die('skip mbstring extension not available');
if (!function_exists('mb_ereg_search_init')) die('skip mb_ereg_search_init() not available');
?>
--FILE--
<?php
mb_ereg_search_init('foo');

var_dump(mb_ereg_search_setpos(3));
var_dump(mb_ereg_search_getpos());

var_dump(mb_ereg_search('\Z'));
var_dump(mb_ereg_search_getpos());
?>
--EXPECT--
bool(true)
int(3)
bool(true)
int(3)
