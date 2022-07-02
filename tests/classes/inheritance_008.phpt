--TEST--
Ensure private methods with the same name are not checked for inheritance rules - static
--FILE--
<?php
class A {
    static private function foo() { }
    private function bar() {}
}
class B extends A {
    private function foo() {}
    static private function bar() {}
}
echo 'OK';
?>
--EXPECT--
OK
