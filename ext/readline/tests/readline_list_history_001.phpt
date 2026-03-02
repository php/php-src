--TEST--
readline_list_history(): Basic test
--EXTENSIONS--
readline
--SKIPIF--
<?php if (!function_exists('readline_list_history')) die("skip"); ?>
--FILE--
<?php

var_dump(readline_list_history());

?>
--EXPECT--
array(0) {
}
