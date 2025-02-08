--TEST--
Pipe operator accepts prefer-by-reference functions.
--FILE--
<?php

$a = ['hello', 'world'];

try {
    $r = $a |> array_multisort(...);
    var_dump($r);
} catch (\Error $e) {
  print $e->getMessage() . PHP_EOL;
}

?>
--EXPECT--
bool(true)
