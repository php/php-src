--TEST--
It is not possible to pass function that expects more parameters even if they are optional but have typehints
--FILE--
<?php

function foo(callable() $cb) {
    $cb();
};
foo(function (int ...$a) { var_dump(1); });

?>
--EXPECTF--
Fatal error: Uncaught TypeError: Argument 1 passed to foo() must be compliant with callable(), incompatible callable(integer ...$a) given, called in %s on line %d and defined in %s:%d
Stack trace:
#0 %s(%d): foo(Object(Closure))
#1 {main}
  thrown in %s on line %d
