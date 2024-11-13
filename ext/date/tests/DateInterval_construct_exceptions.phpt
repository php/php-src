--TEST--
DateInterval constructor exceptions
--INI--
date.timezone=Europe/London
--FILE--
<?php
function check(callable $c)
{
	try {
		var_dump($c());
	} catch (\DateMalformedIntervalStringException $e) {
		echo $e::class, ': ', $e->getMessage(), "\n";
	}
}

check(fn() => new DateInterval(""));
check(fn() => new DateInterval("2007-05-11T15:30:00Z/"));
?>
--EXPECTF--
DateMalformedIntervalStringException: Unknown or bad format ()
DateMalformedIntervalStringException: Failed to parse interval (2007-05-11T15:30:00Z/)
