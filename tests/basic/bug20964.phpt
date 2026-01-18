--TEST--
Bug #20964 fseek with PHP_INT_MIN on php://memory
--FILE--
<?php
$stream = fopen('php://memory', 'r+');

$result = fseek($stream, PHP_INT_MIN, SEEK_END);
var_dump($result);
?>
--EXPECTF--
int(%d)