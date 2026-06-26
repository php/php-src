--TEST--
Reflection: method type parameters separate from class type parameters
--FILE--
<?php
class C<X> {
    public function f<U>(): void {}
}
$rc = new ReflectionClass('C');
$class_ps = $rc->getGenericParameters();
$method_ps = $rc->getMethod('f')->getGenericParameters();
echo count($class_ps), ": ", $class_ps[0]->getName(), "\n";
echo count($method_ps), ": ", $method_ps[0]->getName(), "\n";
?>
--EXPECT--
1: X
1: U
