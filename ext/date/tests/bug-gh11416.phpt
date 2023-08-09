--TEST--
Bug GH-11416: Crash with DatePeriod when uninitialised objects are passed in
--INI--
date.timezone=UTC
--FILE--
<?php
$now = new DateTimeImmutable();

$date = (new ReflectionClass(DateTime::class))->newInstanceWithoutConstructor();
try {
	new DatePeriod($date, new DateInterval('P1D'), 2);
} catch (Error $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}

$date = (new ReflectionClass(DateTime::class))->newInstanceWithoutConstructor();
try {
	new DatePeriod($now, new DateInterval('P1D'), $date);
} catch (Error $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}

echo "OK\n";
?>
--EXPECT--
Error: The DateTimeInterface object has not been correctly initialized by its constructor
Error: The DateTimeInterface object has not been correctly initialized by its constructor
OK
