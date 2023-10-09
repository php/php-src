--TEST--
DateTime uninitialised exceptions
--INI--
date.timezone=Europe/London
--FILE--
<?php
class MyDateTime extends DateTime
{
	function __construct()
	{
	}
}

class MyDateTimeZone extends DateTimeZone
{
	function __construct()
	{
	}
}

function check(callable $c)
{
	try {
		var_dump($c());
	} catch (\DateObjectError $e) {
		echo $e::class, ': ', $e->getMessage(), "\n";
	}
}

$mdt = new MyDateTime();
$mdtz = new MyDateTimeZone();
$dtz = new DateTimeZone("Europe/Kyiv");
$dt = new DateTime("2023-01-16 18:18");

check(fn() => serialize($mdtz));
check(fn() => timezone_name_get($mdtz));
check(fn() => $mdtz->getName());
check(fn() => timezone_offset_get($mdtz, $dt));
check(fn() => $mdtz->getOffset($dt));
check(fn() => timezone_offset_get($dtz, $mdt));
check(fn() => $dtz->getOffset($mdt));
check(fn() => timezone_transitions_get($mdtz, time()));
check(fn() => $mdtz->getTransitions(time()));
check(fn() => timezone_location_get($mdtz,));
check(fn() => $mdtz->getLocation());
?>
--EXPECTF--
DateObjectError: Object of type MyDateTimeZone (inheriting DateTimeZone) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTimeZone (inheriting DateTimeZone) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTimeZone (inheriting DateTimeZone) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTimeZone (inheriting DateTimeZone) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTimeZone (inheriting DateTimeZone) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTime (inheriting DateTime) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTime (inheriting DateTime) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTimeZone (inheriting DateTimeZone) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTimeZone (inheriting DateTimeZone) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTimeZone (inheriting DateTimeZone) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTimeZone (inheriting DateTimeZone) has not been correctly initialized by calling parent::__construct() in its constructor
