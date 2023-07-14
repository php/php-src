--TEST--
Test DateInterval::createFromDateString() function : nonsense data
--FILE--
<?php
try {
	$i = DateInterval::createFromDateString("foobar");
} catch (DateMalformedIntervalStringException $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECTF--
DateMalformedIntervalStringException: Unknown or bad format (foobar) at position 0 (f): The timezone could not be found in the database
