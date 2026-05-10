--TEST--
Scoping: T from class scope is visible in method parameter types
--FILE--
<?php
interface Iface {}
class Box<T : Iface> {
    public function set(T $x): void {}
}
$rm = (new ReflectionClass('Box'))->getMethod('set');
echo $rm->getParameters()[0]->getType()->getName(), "\n";
?>
--EXPECT--
Iface
