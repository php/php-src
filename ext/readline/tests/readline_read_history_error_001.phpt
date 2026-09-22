--TEST--
readline_read_history() function - Error cases
--CREDITS--
Pedro Manoel Evangelista <pedro.evangelista at gmail dot com>
--EXTENSIONS--
readline
--SKIPIF--
<?php if (!READLINE_LIB != "libedit") die('skip READLINE_LIB != "libedit"'); ?>
--FILE--
<?php
var_dump(readline_read_history('nofile'));
?>
--EXPECT--
bool(false)
