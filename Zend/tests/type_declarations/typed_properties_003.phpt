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
Fatal error: Uncaught TypeError: fetching reference to class@anonymous::$int is disallowed in %s:6
Stack trace:
#0 {main}
  thrown in %s on line 6


