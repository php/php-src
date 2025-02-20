--TEST--
visibility
--FILE--
<?php

class Foo {
    private class Bar {
        public function __construct() {
            echo "Inner\n";
        }
    }

    public function __construct() {
        new self::Bar();
    }
}

new Foo();
new Foo::Bar();
?>
--EXPECTF--
Inner

Fatal error: Uncaught Error: Cannot access private property Foo::$Bar in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
