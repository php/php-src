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
DateObjectError: Object of type DateTimeInterface has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type DateTimeInterface has not been correctly initialized by calling parent::__construct() in its constructor
OK
