--TEST--
DateTimeZone compare handler
--FILE--
<?php
class MyDateTimeZone extends DateTimeZone
{
	function __construct()
	{
	}
}

$mdtz = new MyDateTimeZone();
$dtzID = new DateTimeZone("Europe/Kyiv");
$dtzAbbr = new DateTimeZone("CEST");
$dtzUTC = new DateTimeZone("-5:00");


try {
	var_dump($mdtz < $dtzID);
} catch (\DateObjectError $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
	var_dump($dtzID < $mdtz);
} catch (\DateObjectError $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
	var_dump($dtzID < $dtzAbbr);
} catch (\DateException $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
	var_dump($dtzAbbr < $dtzUTC);
} catch (\DateException $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
DateObjectError: Trying to compare uninitialized DateTimeZone objects
DateObjectError: Trying to compare uninitialized DateTimeZone objects
DateException: Cannot compare two different kinds of DateTimeZone objects
DateException: Cannot compare two different kinds of DateTimeZone objects
