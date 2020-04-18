--TEST--
Closure 027: Testing Closure type-hint
--FILE--
<?php

function test(Closure $a) {
    var_dump($a());
}


test(function() { return new stdclass; });

test(function() { });

$a = function($x) use ($y) {};
try {
    test($a);
} catch (Throwable $e) {
    echo "Exception: " . $e->getMessage() . "\n";
}

test(new stdclass);

?>
--EXPECTF--
object(stdClass)#%d (0) {
}
NULL

Warning: Undefined variable $y in %s on line %d
Exception: Too few arguments to function {closure}(), 0 passed in %s on line %d and exactly 1 expected

Fatal error: Uncaught TypeError: test(): Argument #1 ($a) must be of type Closure, stdClass given, called in %s:%d
Stack trace:
#0 %s(%d): test(Object(stdClass))
#1 {main}
  thrown in %s on line %d
