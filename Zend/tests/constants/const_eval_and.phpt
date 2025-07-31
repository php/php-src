--TEST--
Incorrect constant evaluation of and/or (OSS-Fuzz #19255)
--FILE--
<?php
const C = 0 && __namespace__;
var_dump(C);
?>
--EXPECT--
bool(false)
