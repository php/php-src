--TEST--
Test typed properties error condition (fetch uninitialized by reference)
--FILE--
<?php
$thing = new class() {
    public int $int;
};

$var = &$thing->int;
?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot access uninitialized non-nullable property class@anonymous::$int by reference in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
