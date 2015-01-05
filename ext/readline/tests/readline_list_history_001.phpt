--TEST--
readline_list_history(): Basic test
--SKIPIF--
<?php if (!extension_loaded("readline") || !function_exists('readline_list_history')) die("skip"); ?>
--FILE--
<?php

var_dump(readline_list_history());
var_dump(readline_list_history(1));

?>
--EXPECTF--
array(0) {
}

Warning: readline_list_history() expects exactly 0 parameters, 1 given in %s on line %d
NULL
