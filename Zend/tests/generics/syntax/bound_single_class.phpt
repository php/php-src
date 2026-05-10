--TEST--
Generic syntax: bound on a single class type
--FILE--
<?php
class Animal {}
class Box<T : Animal> {}
$p = (new ReflectionClass('Box'))->getGenericParameters()[0];
var_dump($p->hasBound());
echo $p->getBound()->getName(), "\n";
?>
--EXPECT--
bool(true)
Animal
