--TEST--
Testing visibility of object returned by function
--FILE--
<?php

class foo {
    private $test = 1;
}

function test() {
    return new foo;
}

test()->test = 2;

?>
--EXPECTF--
Fatal error: Uncaught Error: Private property foo::$test cannot be accessed from the global scope in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
