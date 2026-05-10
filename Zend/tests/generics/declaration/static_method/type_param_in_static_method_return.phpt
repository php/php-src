--TEST--
Static methods: class type parameter is allowed as a static method return type
--FILE--
<?php
class A<T> {
    public static function foo(): ?T { return null; }
}
echo "ok\n";
?>
--EXPECT--
ok
