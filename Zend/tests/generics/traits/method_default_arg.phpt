--TEST--
Traits: omitting use args fills the trait parameter defaults into method signatures
--FILE--
<?php
trait Defaulted<T = int> {
    public function f(T $x): T { return $x; }
}

class Impl { use Defaulted; }

$rm = (new ReflectionClass('Impl'))->getMethod('f');
echo $rm->getParameters()[0]->getType()->getName(), " -> ",
     $rm->getReturnType()->getName(), "\n";

$i = new Impl;
echo $i->f(42), "\n";
?>
--EXPECT--
int -> int
42
