--TEST--
Scoping: method introduces its own type parameter
--FILE--
<?php
class A {}
class B {}
class Box<T : A> {
    public function morph<U : B>(T $x, U $y): U { return $y; }
}
$rm = (new ReflectionClass('Box'))->getMethod('morph');
echo $rm->getParameters()[0]->getType()->getName(), "\n";
echo $rm->getParameters()[1]->getType()->getName(), "\n";
echo $rm->getReturnType()->getName(), "\n";
?>
--EXPECT--
A
B
B
