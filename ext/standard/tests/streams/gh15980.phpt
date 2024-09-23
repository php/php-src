--TEST--
GH-15980 (Signed integer overflow in main/streams/streams.c)
--FILE--
<?php
$s = fopen(__FILE__, "r");
fseek($s, 1);
var_dump(fseek($s, PHP_INT_MAX, SEEK_CUR));
var_dump(ftell($s));
?>
--EXPECT--
