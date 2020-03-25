--TEST--
Bug #65213 (cannot cast SplFileInfo to boolean)
--FILE--
<?php

$o = new SplFileInfo('.');
var_dump((bool) $o);

?>
--EXPECT--
bool(true)
