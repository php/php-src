--TEST--
GH-17718: Disallow calling abstract `__callStatic()` trampoline on an interface
--FILE--
<?php

interface Foo {
    public static function __callStatic($method, $args);
}

Foo::bar();

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot call abstract method Foo::bar() in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
