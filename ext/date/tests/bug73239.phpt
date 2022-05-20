--TEST--
Bug #73239 (Odd warning/exception message with invalid timezones)
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
Warning: ini_set(): Invalid date.timezone value 'dummy' in %sbug73239.php on line %d
DateTime::__construct(): Invalid date.timezone value 'dummy', we selected the timezone 'UTC' for now.
