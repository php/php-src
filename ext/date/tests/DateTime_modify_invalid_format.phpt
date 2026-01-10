--TEST--
DateTime::modify() with empty string as format
--FILE--
<?php

$datetime = new DateTime;
var_dump(date_modify($datetime, ''));
try {
	var_dump($datetime->modify(''));
} catch (DateMalformedStringException $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECTF--
Warning: date_modify(): Failed to parse time string () at position 0 ( ): Empty string in %s
bool(false)
DateMalformedStringException: DateTime::modify(): Failed to parse time string () at position 0 ( ): Empty string
