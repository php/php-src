--TEST--
DateTimeImmutable::modify() with invalid format
--FILE--
<?php

$datetime = new DateTimeImmutable;
try {
	var_dump($datetime->modify(''));
} catch (DateMalformedStringException $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECTF--
DateMalformedStringException: DateTimeImmutable::modify(): Failed to parse time string () at position 0 ( ): Empty string
