--TEST--
Don't optimize object -> iterable
--FILE--
<?php

function test(object $arg): iterable {
    return $arg;
}
test(new stdClass);

?>
--EXPECTF--
Fatal error: Uncaught TypeError: Return value of test() must be iterable, object returned in %s:%d
Stack trace:
#0 %s(%d): test(Object(stdClass))
#1 {main}
  thrown in %s on line %d
