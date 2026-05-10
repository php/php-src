--TEST--
Inherited methods: substitution composes through a generic intermediate class
--FILE--
<?php
class Base<T> {
    public function take(T $x): T { return $x; }
}

class Mid<U> extends Base<U> {}
class Leaf extends Mid<float> {}

$rm = (new ReflectionClass('Leaf'))->getMethod('take');
echo $rm->getParameters()[0]->getType()->getName(), " -> ",
     $rm->getReturnType()->getName(), "\n";
?>
--EXPECT--
float -> float
