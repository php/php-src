--TEST--
Test bindec() function : strange literals
--FILE--
<?php

var_dump(bindec('0b'));
var_dump(bindec('0B'));
var_dump(bindec(''));

?>
--EXPECT--
int(0)
int(0)
int(0)
