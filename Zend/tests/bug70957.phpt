--TEST--
Bug #70957 (self::class can not be resolved with reflection for abstract class)
--FILE--
<?php

abstract class Foo
{
    function bar($a = self::class) {}
}

trait T {
    public function bar() {
    }
}

class Bar extends Foo
{
    use T;
}
?>
--EXPECTF--
Strict Standards: Declaration of T::bar() should be compatible with Foo::bar($a = 'Foo') in %sbug70957.php on line %d
