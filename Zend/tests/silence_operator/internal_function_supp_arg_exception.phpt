--TEST--
Silencing an internal function supplementary argument throwing an Exception
--FILE--
<?php

function foo() {
    throw new Exception();
    return 1;
}

$var = error_get_last(@foo());

var_dump($var);

echo "Done\n";
?>
--EXPECTF--
Fatal error: Uncaught ArgumentCountError: error_get_last() expects exactly 0 arguments, 1 given in %s:%d
Stack trace:
#0 %s(%d): error_get_last(NULL)
#1 {main}
  thrown in %s on line %d
