--TEST--
is_callable() with trampoline should not caused UAF
--FILE--
<?php

class B {}
class A extends B {
    public function bar($func) {
        var_dump(is_callable(array('B', 'foo')));
    }

    public function __call($func, $args) {
    }
}

class X {
    public static function __callStatic($func, $args) {
    }
}

$a = new A();
// Extra X::foo() wrapper to force use of allocated trampoline.
X::foo($a->bar('foo'));

?>
--EXPECT--
bool(false)
