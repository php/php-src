--TEST--
PFA returns by val if the actual function does
--FILE--
<?php

function foo($a, $b) {
    $b = 2;
}

$a = ['unchanged because foo() doesn\'t take by reference'];
$b = &$a[0];

$foo = foo(1, ?);
$foo($b);

var_dump($a, $b);

?>
--EXPECT--
array(1) {
  [0]=>
  &string(49) "unchanged because foo() doesn't take by reference"
}
string(49) "unchanged because foo() doesn't take by reference"
