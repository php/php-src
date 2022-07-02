--TEST--
Testing declare statement with ticks with callback arguments
--FILE--
<?php
register_tick_function(function (stdClass $object, array $array) {
    echo "tick: ", get_class($object), " ", count($array), "\n";
}, new \stdClass(), [1, 2, 3]);
function foo() { }

declare(ticks=1) {

$statement;
foo();

}
?>
--EXPECT--
tick: stdClass 3
tick: stdClass 3
