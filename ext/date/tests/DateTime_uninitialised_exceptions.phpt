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

function check(callable $c)
{
	try {
		var_dump($c());
	} catch (\DateObjectError $e) {
		echo $e::class, ': ', $e->getMessage(), "\n";
	}
}

$mdt = new MyDateTime();
$dt = new DateTime();
$di = DateInterval::createFromDateString("tomorrow");
$dtz = new DateTimeZone("Europe/Kyiv");

check(fn() => DateTime::createFromInterface($mdt));
check(fn() => DateTimeImmutable::createFromMutable($mdt));
check(fn() => serialize($mdt));
check(fn() => date_format($mdt, DateTime::ISO8601));
check(fn() => $mdt->format(DateTime::ISO8601));
check(fn() => date_modify($mdt, "+1 day"));
check(fn() => $mdt->modify("+1 day"));
check(fn() => $mdt->add($di));
check(fn() => $mdt->sub($di));
check(fn() => date_timezone_get($mdt));
check(fn() => $mdt->getTimeZone());
check(fn() => date_timezone_set($mdt, $dtz));
check(fn() => $mdt->setTimeZone($dtz));
check(fn() => date_offset_get($mdt));
check(fn() => $mdt->getOffset());
check(fn() => date_time_set($mdt, 17, 59, 53));
check(fn() => $mdt->setTime(17, 59, 53));
check(fn() => date_date_set($mdt, 2023, 1, 16));
check(fn() => $mdt->setDate(2023, 1, 16));
check(fn() => date_isodate_set($mdt, 2023, 3, 1));
check(fn() => $mdt->setISODate(2023, 3, 1));
check(fn() => date_timestamp_set($mdt, time()));
check(fn() => $mdt->setTimestamp(time()));
check(fn() => date_timestamp_get($mdt));
check(fn() => $mdt->getTimestamp());
check(fn() => date_diff($dt, $mdt));
check(fn() => date_diff($mdt, $dt));
check(fn() => date_diff($mdt, $mdt));
check(fn() => $dt->diff($mdt));
check(fn() => $mdt->diff($dt));
check(fn() => $mdt->diff($mdt));
?>
--EXPECTF--
DateObjectError: Object of type MyDateTime (inheriting DateTime) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTime (inheriting DateTime) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTime (inheriting DateTime) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTime (inheriting DateTime) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTime (inheriting DateTime) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTime (inheriting DateTime) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTime (inheriting DateTime) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTime (inheriting DateTime) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTime (inheriting DateTime) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTime (inheriting DateTime) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTime (inheriting DateTime) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTime (inheriting DateTime) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTime (inheriting DateTime) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTime (inheriting DateTime) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTime (inheriting DateTime) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTime (inheriting DateTime) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTime (inheriting DateTime) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTime (inheriting DateTime) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTime (inheriting DateTime) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTime (inheriting DateTime) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTime (inheriting DateTime) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTime (inheriting DateTime) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTime (inheriting DateTime) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTime (inheriting DateTime) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTime (inheriting DateTime) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTime (inheriting DateTime) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTime (inheriting DateTime) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTime (inheriting DateTime) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTime (inheriting DateTime) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTime (inheriting DateTime) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTime (inheriting DateTime) has not been correctly initialized by calling parent::__construct() in its constructor
