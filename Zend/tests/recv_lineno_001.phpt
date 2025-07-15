--TEST--
RECV[_INIT] lineno is function start
--FILE--
<?php

class Foo {
    public function __construct(
        public $x = FOO,
    ) {}
}

new Foo();

?>
--EXPECTF--
Fatal error: Uncaught Error: Undefined constant "FOO" in %s:4
Stack trace:
#0 %s(9): Foo->__construct()
#1 {main}
  thrown in %s on line 4
