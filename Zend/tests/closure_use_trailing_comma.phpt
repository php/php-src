--TEST--
Closure use list can have trailing commas
--FILE--
<?php

$b = 'test';
$fn = function () use (
    $b,
    &$a,
) {
    $a = $b;
};
$fn();
echo "$a\n";
?>
--EXPECT--
test
