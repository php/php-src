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
Fatal error: Uncaught Error: forward_static_call() cannot be called from the global scope in %s:%d
Stack trace:
#0 %s(%d): forward_static_call(Array)
#1 %s(%d): test()
#2 {main}
  thrown in %s on line %d
