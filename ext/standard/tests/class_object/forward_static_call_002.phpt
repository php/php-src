--TEST--
forward_static_call() from outside of a class method.
--FILE--
<?php

class A
{
    public static function test() {
        echo "A\n";
    }
}

function test() {
    forward_static_call(array('A', 'test'));
}

test();

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot call forward_static_call() when no class scope is active in %s:%d
Stack trace:
#0 %s(%d): forward_static_call(Array)
#1 %s(%d): test()
#2 {main}
  thrown in %s on line %d
