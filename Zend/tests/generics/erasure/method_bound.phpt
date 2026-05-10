--TEST--
Erasure: method-level type parameter erased
--FILE--
<?php
class Stringable2 {}
class C {
    public function m<U : Stringable2>(U $x): U { return $x; }
}
$rt = (new ReflectionClass('C'))->getMethod('m')->getReturnType();
echo $rt->getName(), "\n";
?>
--EXPECT--
Stringable2
