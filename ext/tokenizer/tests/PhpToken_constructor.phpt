--TEST--
PhpToken constructor
--SKIPIF--
<?php if (!extension_loaded("tokenizer")) print "skip tokenizer extension not enabled"; ?>
--FILE--
<?php

$token = new PhpToken(300, 'function');
var_dump($token);
$token = new PhpToken(300, 'function', 10);
var_dump($token);
$token = new PhpToken(300, 'function', 10, 100);
var_dump($token);

?>
--EXPECT--
object(PhpToken)#1 (4) {
  ["id"]=>
  int(300)
  ["text"]=>
  string(8) "function"
  ["line"]=>
  int(-1)
  ["pos"]=>
  int(-1)
}
object(PhpToken)#2 (4) {
  ["id"]=>
  int(300)
  ["text"]=>
  string(8) "function"
  ["line"]=>
  int(10)
  ["pos"]=>
  int(-1)
}
object(PhpToken)#1 (4) {
  ["id"]=>
  int(300)
  ["text"]=>
  string(8) "function"
  ["line"]=>
  int(10)
  ["pos"]=>
  int(100)
}
