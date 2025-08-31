--TEST--
Trying to access a constant on trait via the name of trait causes a Fatal error
--FILE--
<?php
trait Foo {
    const A = 42;
}

class Bar {
    use Foo;
}

echo Foo::A, PHP_EOL;
?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot access trait constant Foo::A directly in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
