--TEST--
Static methods: class type parameter is allowed in a nullable type on a static method
--FILE--
<?php
class A<T> {
    public static function foo(?T $x): void {}
}
echo "ok\n";
?>
--EXPECT--
ok
