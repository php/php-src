--TEST--
Test bug GH-9763: DateTimeZone ctr mishandles input and adds null byte if the argument is an offset larger than 100*60 minutes
--FILE--
<?php
date_default_timezone_set('UTC');

foreach ( [ '+99:60', '+99:62', '-99:62', '-99:60', '+9960', '-9960', '+9959', '-9959' ] as $test )
{
	echo "Testing {$test}: ";
	try {
		$d = new DateTimeZone($test);
		echo $d->getName(), "\n";
	} catch (Exception $e) {
		echo $e->getMessage(), "\n";
	}
}


?>
--EXPECT--
Testing +99:60: DateTimeZone::__construct(): Timezone offset is out of range (+99:60)
Testing +99:62: DateTimeZone::__construct(): Timezone offset is out of range (+99:62)
Testing -99:62: DateTimeZone::__construct(): Timezone offset is out of range (-99:62)
Testing -99:60: DateTimeZone::__construct(): Timezone offset is out of range (-99:60)
Testing +9960: DateTimeZone::__construct(): Timezone offset is out of range (+9960)
Testing -9960: DateTimeZone::__construct(): Timezone offset is out of range (-9960)
Testing +9959: +99:59
Testing -9959: -99:59
