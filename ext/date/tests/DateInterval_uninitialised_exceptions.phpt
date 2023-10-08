--TEST--
DateInterval uninitialised exceptions
--INI--
date.timezone=Europe/London
--FILE--
<?php
class MyDateInterval extends DateInterval
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

$mdi = new MyDateInterval();

check(fn() => serialize($mdi));
check(fn() => $mdi->format("Y-m-d"));
?>
--EXPECTF--
DateObjectError: Object of type MyDateInterval (inheriting DateInterval) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateInterval (inheriting DateInterval) has not been correctly initialized by calling parent::__construct() in its constructor
