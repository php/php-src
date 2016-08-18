--TEST--
Bug #69674 (SIGSEGV array.c:953)
--FILE--
<?php
var_dump(current(array_keys(array())));
?>
--EXPECT--
bool(false)
