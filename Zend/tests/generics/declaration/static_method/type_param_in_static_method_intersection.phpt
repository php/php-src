--TEST--
Static methods: class type parameter is allowed in an intersection type on a static method
--FILE--
<?php
class A<T: Iterator> {
    public static function foo(T&Countable $x): void {}
}
echo "ok\n";
?>
--EXPECT--
ok
