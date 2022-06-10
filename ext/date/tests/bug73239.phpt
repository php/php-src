--TEST--
Bug #73239 (Odd warning/exception message with invalid timezones)
--INI--
date.timezone=UTC
--FILE--
<?php
ini_set('date.timezone', 'dummy');
try {
	$dt = new DateTime('now');
} catch (Exception $e) {
	echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
Warning: ini_set(): Invalid date.timezone value 'dummy', using 'UTC' instead in %sbug73239.php on line %d
