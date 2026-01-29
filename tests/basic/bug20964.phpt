--TEST--
Bug #20964 fseek with PHP_INT_MIN on php://memory
--FILE--
<?php
$stream = fopen('php://memory', 'r+');

$result = fseek($stream, PHP_INT_MIN, SEEK_END);
var_dump($result);
?>
--EXPECTF--
Fatal error: Uncaught ValueError: fseek(): Argument #2 ($offset) must be greater than -%d in %s:%d
Stack trace:
#0 %s(%d): fseek(Resource id #%d, -%d, %d)
#1 {main}
  thrown in %s on line %d