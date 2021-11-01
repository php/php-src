--TEST--
Unreachable code elimination when match argument is a phi node
--FILE--
<?php
$x = true;
match ($x and true or true) {
    false => $x
};
?>
--EXPECTF--
Fatal error: Uncaught UnhandledMatchError: Unhandled match case true in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
