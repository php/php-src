--TEST--
GH-15980 (Signed integer overflow in main/streams/streams.c)
--FILE--
<?php
$s = fopen(__FILE__, "r");
fseek($s, 1);
fseek($s, PHP_INT_MAX, SEEK_CUR);
var_dump(ftell($s) > 1);
?>
--EXPECT--
bool(true)
