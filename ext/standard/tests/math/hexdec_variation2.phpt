--TEST--
Test hexdec() function : strange literals
--FILE--
<?php

var_dump(hexdec('0x'));
var_dump(hexdec('0X'));
var_dump(hexdec(''));

?>
--EXPECT--
int(0)
int(0)
int(0)
