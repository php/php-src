--TEST--
Bug GH-11416: Crash with DatePeriod when uninitialised objects are passed in
--INI--
date.timezone=UTC
--FILE--
<?php
$now = new DateTimeImmutable();
$simpleInterval = new DateInterval("P2D");

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

$date = (new ReflectionClass(DateTime::class))->newInstanceWithoutConstructor();
$dateperiod = (new ReflectionClass(DatePeriod::class))->newInstanceWithoutConstructor();
$dateinterval = (new ReflectionClass(DateInterval::class))->newInstanceWithoutConstructor();
try {
	$dateperiod->__unserialize(['start' => $date]);
} catch (Error $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}

try {
	$dateperiod->__unserialize(['start' => $now, 'end' => $date]);
} catch (Error $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}

try {
	$dateperiod->__unserialize(['start' => $now, 'end' => $now, 'current' => $date]);
} catch (Error $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}

try {
	$dateperiod->__unserialize(['start' => $now, 'end' => $now, 'current' => $now, 'interval' => $dateinterval]);
} catch (Error $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}

try {
	$dateperiod->__unserialize([
		'start' => $now, 'end' => $now, 'current' => $now, 'interval' => $simpleInterval,
		'recurrences' => 2, 'include_start_date' => true, 'include_end_date' => true,
	]);
	echo "DatePeriod::__unserialize: SUCCESS\n";
} catch (Error $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}
echo "OK\n";
?>
--EXPECT--
DateObjectError: Object of type DateTimeInterface has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type DateTimeInterface has not been correctly initialized by calling parent::__construct() in its constructor
Error: Invalid serialization data for DatePeriod object
Error: Invalid serialization data for DatePeriod object
Error: Invalid serialization data for DatePeriod object
Error: Invalid serialization data for DatePeriod object
DatePeriod::__unserialize: SUCCESS
OK
