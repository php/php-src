--TEST--
Pipe operator and array_map() with aliased assert()
--FILE--
<?php

use function assert as a;

var_dump(true |> a(...));
var_dump(array_map(a(...), [true]));

try {
    false |> a(...);
} catch (AssertionError $e) {
    echo $e::class, ': ', $e->getMessage() ?: '(no message)', "\n";
}

try {
    array_map(a(...), [false]);
} catch (AssertionError $e) {
    echo $e::class, ': ', $e->getMessage() ?: '(no message)', "\n";
}

?>
--EXPECT--
bool(true)
array(1) {
  [0]=>
  bool(true)
}
AssertionError: (no message)
AssertionError: (no message)
