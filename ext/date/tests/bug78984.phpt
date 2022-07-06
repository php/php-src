--TEST--
Bug #78984: DateTimeZone accepting invalid UTC timezones
--INI--
date.timezone=UTC
--FILE--
<?php
$tests = [ '-3', '+3', '+1345', '+30157' ];

foreach ($tests as $test)
{
	echo "Testing for '{$test}': ";
	try {
		new \DateTimeZone($test);
		echo "OK\n";
	} catch (Exception $e)	{
		echo $e->getMessage(), "\n";
	}
}
?>
--EXPECT--
Testing for '-3': OK
Testing for '+3': OK
Testing for '+1345': OK
Testing for '+30157': DateTimeZone::__construct(): Unknown or bad timezone (+30157)
