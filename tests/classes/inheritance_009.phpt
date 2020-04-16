--TEST--
Ensure private methods with the same name are not checked for inheritance rules - abstract
--FILE--
<?php
class A {
    private function test() {}
}
abstract class B extends A {
    abstract function test();
}
echo 'OK';
?>
--EXPECT--
OK
