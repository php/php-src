--TEST--
Static methods: class type parameter is allowed as a bound on a static method's own type parameter
--FILE--
<?php
class A<T> {
    public static function foo<U: T>(U $x): void {}
}
echo "ok\n";
?>
--EXPECT--
ok
