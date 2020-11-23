--TEST--
Bug #40621 (Crash when constructor called inappropriately (statically))
--FILE--
<?php

class Foo {
    private function __construct() { }
    function get() {
        self::__construct();
    }
}

Foo::get();

echo "Done\n";
?>
--EXPECTF--
Fatal error: Uncaught Error: Non-static method Foo::get() cannot be called statically in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
