--TEST--
readline_list_history(): Basic test
--SKIPIF--
<?php if (!extension_loaded("readline") || !function_exists('readline_list_history')) die("skip"); ?>
--FILE--
<?php

var_dump(readline_list_history());

?>
--EXPECTF--
array(0) {
}
