--TEST--
readline_add_history(): Basic test
--EXTENSIONS--
readline
--SKIPIF--
<?php if (!function_exists('readline_list_history')) die("skip"); ?>
--FILE--
<?php

var_dump(readline_add_history('foo'));
var_dump(readline_list_history());
var_dump(readline_add_history(''));
var_dump(readline_list_history());
var_dump(readline_clear_history());

?>
--EXPECT--
bool(true)
array(1) {
  [0]=>
  string(3) "foo"
}
bool(true)
array(2) {
  [0]=>
  string(3) "foo"
  [1]=>
  string(0) ""
}
bool(true)
