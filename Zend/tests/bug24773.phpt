--TEST--
Bug #24773 (unset() of integers treated as arrays causes a crash)
--FILE--
<?php
	$array = 'test';
	unset($array["lvl1"]["lvl2"]["b"]);
?>
--EXPECTF--
Fatal error: Cannot unset string offsets in %s on line %d
