--TEST--
GH-20906 (Assertion failure when messing up output buffers) - highlight_string
--CREDITS--
vi3tL0u1s
--FILE--
<?php
class A {
    function __destruct() {
        highlight_string(__FILE__, true);
        echo "x";
        $c = new A;
        ob_start(function () use ($c) { return '/'; }, 1);
        ob_start(function () use (&$c) { $c = new A; return '/'; }, 1);
    }
}

new A;
?>
--EXPECTF--
%a
Fatal error: highlight_string(): Cannot use output buffering in output buffering display handlers in %s on line %d
