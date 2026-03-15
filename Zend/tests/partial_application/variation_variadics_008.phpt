--TEST--
PFA variation: variadics wrong signature checked
--FILE--
<?php
function foo(...$c) {}

$fn = foo(?);

try {
    $fn(?, ?);
} catch (Error $ex) {
    echo $ex->getMessage() . PHP_EOL;
}
?>
--EXPECTF--
Partial application of {closure:%s:%d}() expects at most 1 arguments, 2 given
