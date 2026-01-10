--TEST--
GH-20183: Stale EG(opline_before_exception) pointer through eval
--CREDITS--
Arnaud Le Blanc <lbarnaud@php.net>
--FILE--
<?php

function gen() {
    try {
        yield 1;
    } finally {
        eval('try { throw new Error(); } catch (Error) {}');
        debug_print_backtrace();
    }
}

class A {
    private $gen;
    function __construct() {
        $this->gen = gen();
        $this->gen->rewind();
    }
}

B::$a = new A();

?>
--EXPECTF--
#0 %s(20): gen()

Fatal error: Uncaught Error: Class "B" not found in %s:20
Stack trace:
#0 {main}
  thrown in %s on line 20
