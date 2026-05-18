--TEST--
Erasure: child overriding generic parent method works on erased signature
--FILE--
<?php
class Animal {}
class Parent2 {
    public function f<T : Animal>(T $x): T { return $x; }
}
class Child extends Parent2 {
    public function f<U : Animal>(U $x): U { return $x; }
}
$pm = (new ReflectionClass('Parent2'))->getMethod('f');
$cm = (new ReflectionClass('Child'))->getMethod('f');
echo $pm->getParameters()[0]->getType()->getName(), "\n";
echo $cm->getParameters()[0]->getType()->getName(), "\n";
?>
--EXPECT--
Animal
Animal
