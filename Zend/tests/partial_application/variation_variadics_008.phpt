--TEST--
Partial application variation variadics wrong signature checked
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
--EXPECT--
too many arguments or placeholders for application of Closure::__invoke, 2 given and a maximum of 1 expected

