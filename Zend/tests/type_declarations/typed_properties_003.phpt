--TEST--
Test typed properties error condition (fetch reference)
--FILE--
<?php
$thing = new class() {
	public int $int;
};

$var = &$thing->int;
?>
--EXPECTF--
Fatal error: Uncaught TypeError: Typed property class@anonymous::$int must not be referenced in %s:6
Stack trace:
#0 {main}
  thrown in %s on line 6


