--TEST--
Relative class types from traits should be resolved when used inside a class
--FILE--
<?php

trait TraitCompiled {
    public self $foo;
    public function bar(): self { return new self; }
}

class A {
    use TraitCompiled;
}

$a1 = new A();
$a2 = $a1->bar();
var_dump($a2);

$methodInfo = ReflectionMethod::createFromMethodName("A::bar");
var_dump($methodInfo->getReturnType()->getName());
$property = new ReflectionProperty('A', 'foo');
var_dump($property->getType()->getName());

$methodInfo = ReflectionMethod::createFromMethodName("TraitCompiled::bar");
var_dump($methodInfo->getReturnType()->getName());
$property = new ReflectionProperty('TraitCompiled', 'foo');
var_dump($property->getType()->getName());

?>
DONE
--EXPECT--
object(A)#2 (0) {
  ["foo"]=>
  uninitialized(A)
}
string(1) "A"
string(1) "A"
string(4) "self"
string(4) "self"
DONE
