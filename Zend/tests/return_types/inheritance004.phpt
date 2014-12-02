--TEST--
Compile-time return type invariance; self and parent

--FILE--
<?php

class A {
    function foo(): self {}
}

class B extends A {
    function foo(): parent {}
}

--EXPECT--
