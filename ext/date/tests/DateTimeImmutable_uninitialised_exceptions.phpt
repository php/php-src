--TEST--
DateTime uninitialised exceptions
--INI--
date.timezone=Europe/London
--FILE--
<?php
class MyDateTimeImmutable extends DateTimeImmutable
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

$mdti = new MyDateTimeImmutable();
$dt = new DateTimeImmutable();
$di = DateInterval::createFromDateString("tomorrow");
$dtz = new DateTimeZone("Europe/Kyiv");

check(fn() => DateTimeImmutable::createFromInterface($mdti));
check(fn() => DateTime::createFromImmutable($mdti));
check(fn() => serialize($mdti));
check(fn() => date_format($mdti, DateTime::ISO8601));
check(fn() => $mdti->format(DateTime::ISO8601));
check(fn() => $mdti->modify("+1 day"));
check(fn() => $mdti->add($di));
check(fn() => $mdti->sub($di));
check(fn() => date_timezone_get($mdti));
check(fn() => $mdti->getTimeZone());
check(fn() => $mdti->setTimeZone($dtz));
check(fn() => date_offset_get($mdti));
check(fn() => $mdti->getOffset());
check(fn() => $mdti->setTime(17, 59, 53));
check(fn() => $mdti->setDate(2023, 1, 16));
check(fn() => $mdti->setISODate(2023, 3, 1));
check(fn() => $mdti->setTimestamp(time()));
check(fn() => date_timestamp_get($mdti));
check(fn() => $mdti->getTimestamp());
check(fn() => date_diff($dt, $mdti));
check(fn() => date_diff($mdti, $dt));
check(fn() => date_diff($mdti, $mdti));
check(fn() => $dt->diff($mdti));
check(fn() => $mdti->diff($dt));
check(fn() => $mdti->diff($mdti));
?>
--EXPECTF--
DateObjectError: Object of type MyDateTimeImmutable (inheriting DateTimeImmutable) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTimeImmutable (inheriting DateTimeImmutable) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTimeImmutable (inheriting DateTimeImmutable) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTimeImmutable (inheriting DateTimeImmutable) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTimeImmutable (inheriting DateTimeImmutable) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTimeImmutable (inheriting DateTimeImmutable) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTimeImmutable (inheriting DateTimeImmutable) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTimeImmutable (inheriting DateTimeImmutable) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTimeImmutable (inheriting DateTimeImmutable) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTimeImmutable (inheriting DateTimeImmutable) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTimeImmutable (inheriting DateTimeImmutable) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTimeImmutable (inheriting DateTimeImmutable) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTimeImmutable (inheriting DateTimeImmutable) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTimeImmutable (inheriting DateTimeImmutable) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTimeImmutable (inheriting DateTimeImmutable) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTimeImmutable (inheriting DateTimeImmutable) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTimeImmutable (inheriting DateTimeImmutable) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTimeImmutable (inheriting DateTimeImmutable) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTimeImmutable (inheriting DateTimeImmutable) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTimeImmutable (inheriting DateTimeImmutable) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTimeImmutable (inheriting DateTimeImmutable) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTimeImmutable (inheriting DateTimeImmutable) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTimeImmutable (inheriting DateTimeImmutable) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTimeImmutable (inheriting DateTimeImmutable) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTimeImmutable (inheriting DateTimeImmutable) has not been correctly initialized by calling parent::__construct() in its constructor
