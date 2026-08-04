--TEST--
Declaring a magic method with wrong case is a compile error (__callstatic)
--FILE--
<?php
class Foo {
    public static function __callstatic(string $name, array $args): mixed { return null; }
}
?>
--EXPECTF--
Fatal error: Method Foo::__callstatic() must be spelled __callStatic() in %s on line %d
