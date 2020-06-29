--TEST--
readline_clear_history(): Basic test
--SKIPIF--
<?php if (!extension_loaded("readline")) die("skip"); ?>
--FILE--
<?php

var_dump(readline_clear_history());

?>
--EXPECT--
bool(true)
