--TEST--
GH-15980 (Signed integer overflow in main/streams/streams.c)
--FILE--
<?php
$s = fopen(__FILE__, "r");
fseek($s, 1);
$seekres = fseek($s, PHP_INT_MAX, SEEK_CUR);
$tellres = ftell($s);
var_dump($seekres === -1 || $tellres > 1);
?>
--EXPECT--
bool(true)
