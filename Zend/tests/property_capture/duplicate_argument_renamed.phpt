--TEST--
Can capture the same variable twice if renamed
--FILE--
<?php
$a = 42;
$foo = new class use ($a as $prop1, $a as $prop2) {};
var_dump($foo);
?>
--EXPECT--
object(class@anonymous)#1 (2) {
  ["prop1"]=>
  int(42)
  ["prop2"]=>
  int(42)
}
