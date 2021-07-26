--TEST--
Don't optimize object -> iterable
--EXTENSIONS--
opcache
--FILE--
<?php

function test(object $arg): iterable {
    return $arg;
}
test(new stdClass);

?>
--EXPECTF--
Fatal error: Uncaught TypeError: test(): Return value must be of type Traversable|array, stdClass returned in %s:%d
Stack trace:
#0 %s(%d): test(Object(stdClass))
#1 {main}
  thrown in %s on line %d
