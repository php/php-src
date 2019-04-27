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

class D {
    public int $a;
    public float $b;
}

var_dump(unserialize('O:1:"A":2:{s:1:"a";i:1;s:1:"b";R:2;}'));
var_dump(unserialize('O:1:"B":2:{s:1:"a";i:1;s:1:"b";R:2;}'));

try {
    var_dump(unserialize('O:1:"A":2:{s:1:"a";N;s:1:"b";R:2;}'));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(unserialize('O:1:"B":2:{s:1:"a";N;s:1:"b";R:2;}'));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(unserialize('O:1:"C":2:{s:1:"a";i:1;s:1:"b";R:2;}'));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(unserialize('O:1:"C":2:{s:1:"b";s:1:"x";s:1:"a";R:2;}'));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(unserialize('O:1:"D":2:{s:1:"a";i:1;s:1:"b";R:2;}'));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
object(A)#1 (2) {
  ["a"]=>
  &int(1)
  ["b"]=>
  &int(1)
}
object(B)#1 (2) {
  ["a"]=>
  &int(1)
  ["b"]=>
  &int(1)
}
Typed property A::$a must be int, null used
Typed property B::$b must be int, null used
Typed property C::$b must be string, int used
Typed property C::$a must be int, string used
Reference with value of type int held by property D::$a of type int is not compatible with property D::$b of type float
