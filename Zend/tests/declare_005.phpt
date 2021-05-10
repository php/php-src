--TEST--
Testing declare statement with ticks
--FILE--
<?php
register_tick_function(function () { echo "tick\n"; });
function foo() { }

declare(ticks=1) {

$statement;
foo();

}
?>
--EXPECTF--
Deprecated: declare(ticks) is deprecated in %s on line %d

Deprecated: Function register_tick_function() is deprecated in %s on line %d
tick
tick
