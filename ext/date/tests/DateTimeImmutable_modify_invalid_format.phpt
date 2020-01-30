--TEST--
DateTimeImmutable::modify() with invalid format
--FILE--
<?php

$datetime = new DateTimeImmutable;
var_dump($datetime->modify(''));

?>
--EXPECTF--
Warning: DateTimeImmutable::modify(): Failed to parse time string () at position 0 ( in %s on line %d
bool(false)
