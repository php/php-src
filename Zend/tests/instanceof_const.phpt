--TEST--
Instanceof on literals returns false
--FILE--
<?php

var_dump("abc" instanceof stdclass);

?>
--EXPECT--
bool(false)
