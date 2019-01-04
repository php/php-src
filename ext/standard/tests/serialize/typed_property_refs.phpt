--TEST--
unserialize with references to typed properties shall skip the references or fail
--FILE--
<?php

class A {
	public int $a;
	public $b;
}

class B {
	public $a;
	public int $b;
}

class C {
	public int $a;
	public string $b;
}

var_dump(unserialize('O:1:"A":2:{s:1:"a";i:1;s:1:"b";R:2;}'));
var_dump(unserialize('O:1:"B":2:{s:1:"a";i:1;s:1:"b";R:2;}'));
var_dump(unserialize('O:1:"A":2:{s:1:"a";N;s:1:"b";R:2;}'));
var_dump(unserialize('O:1:"B":2:{s:1:"a";N;s:1:"b";R:2;}'));
var_dump(unserialize('O:1:"C":2:{s:1:"a";i:1;s:1:"b";R:2;}'));
var_dump(unserialize('O:1:"C":2:{s:1:"b";s:1:"x";s:1:"a";R:2;}'));

?>
--EXPECTF--
object(A)#1 (2) {
  ["a"]=>
  &int(1)
  ["b"]=>
  &int(1)
}

Notice: unserialize(): Error at offset 35 of 36 bytes in %s on line %d
bool(false)

Notice: unserialize(): Error at offset 21 of 34 bytes in %s on line %d
bool(false)

Notice: unserialize(): Error at offset 33 of 34 bytes in %s on line %d
bool(false)

Notice: unserialize(): Error at offset 35 of 36 bytes in %s on line %d
bool(false)

Notice: unserialize(): Error at offset 39 of 40 bytes in %s on line %d
bool(false)
