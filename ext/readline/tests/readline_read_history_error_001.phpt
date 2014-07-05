--TEST--
readline_read_history() function - Error cases
--SKIPIF--
<?php if (!extension_loaded("readline") || !function_exists('readline_read_history')) die("skip");
--FILE--
<?php
var_dump(readline_read_history());
var_dump(readline_read_history('nofile'));
?>
--EXPECT--
bool(false)
bool(false)
