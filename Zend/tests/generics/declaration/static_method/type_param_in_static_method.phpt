--TEST--
Static methods: class type parameter is allowed in a static method signature
--FILE--
<?php
class A<T> {
    public static function foo(T $a): void {}
}
echo "ok\n";
?>
--EXPECT--
ok
