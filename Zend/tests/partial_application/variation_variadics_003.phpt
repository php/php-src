--TEST--
Partial application variation variadics interactions
--FILE--
<?php
function foo($a, $b) {
    var_dump(func_get_args());
}

$foo = foo(?, ?);

try {
    $foo(1, 2, 3); // FAIL, 2 expected, 3 given
} catch (Error $ex) {
    printf("%s\n", $ex->getMessage());
}

try {
    $foo = foo(?, ?, ?); // FAIL 2 expected, 3 given
} catch (Error $ex) {
    printf("%s\n", $ex->getMessage());
}

function bar($a, $b, ...$c) {
    var_dump(func_get_args());
}

$bar = bar(?, ?);

try {
    $bar(1, 2, 3); // FAIL 3 given, maximum 2 expected
} catch (Error $ex) {
    printf("%s\n", $ex->getMessage());
}

$foo = foo(?, ?, ...);

$foo(1, 2, 3); // OK
?>
--EXPECTF--
too many arguments for application of foo, 3 given and a maximum of 2 expected, declared in %s on line %d
too many arguments and or place holders for application of foo, 3 given and a maximum of 2 expected, declared in %s on line %d
too many arguments for application of bar, 3 given and a maximum of 2 expected, declared in %s on line %d
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
