--TEST--
Bug #71063 (filter_input(INPUT_ENV, ..) does not work)
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--ENV--
PATH=/
--INI--
variables_order=E
--FILE--
<?php
var_dump(count($_ENV['PATH']) > 0);
var_dump(count(filter_input(INPUT_ENV, 'PATH')) > 0);
?>
--EXPECT--
bool(true)
bool(true)
