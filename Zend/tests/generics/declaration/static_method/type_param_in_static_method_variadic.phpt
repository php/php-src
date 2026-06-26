--TEST--
Static methods: class type parameter is allowed in a variadic static method parameter
--FILE--
<?php
class A<T> {
    public static function foo(T ...$xs): void {}
}
echo "ok\n";
?>
--EXPECT--
ok
