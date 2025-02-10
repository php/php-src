--TEST--
Pipe binds higher than comparison
--FILE--
<?php

function _test1(int $a): int {
    return $a * 2;
}

// This will error without the parens,
// as it will try to compare a closure to an int first.
$res1 = (5 |> _test1(...)) == 10 ;
var_dump($res1);

?>
--EXPECT--
bool(true)
