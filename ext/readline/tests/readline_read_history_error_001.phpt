--TEST--
readline_read_history() function - Error cases
--CREDITS--
Pedro Manoel Evangelista <pedro.evangelista at gmail dot com>
--EXTENSIONS--
readline
--FILE--
<?php
var_dump(readline_read_history('nofile'));
?>
--EXPECT--
bool(false)
