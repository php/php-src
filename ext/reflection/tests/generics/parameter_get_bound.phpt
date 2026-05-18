--TEST--
Reflection: ReflectionGenericTypeParameter::getBound() returns the bound type or throws when no bound
--FILE--
<?php
class A<X, Y : object> {}
$ps = (new ReflectionClass('A'))->getGenericParameters();
var_dump($ps[0]->hasBound());
try {
    $ps[0]->getBound();
} catch (ReflectionException $e) {
    echo $e->getMessage(), "\n";
}
echo $ps[1]->getBound()->getName(), "\n";
?>
--EXPECT--
bool(false)
Type parameter X has no bound
object
