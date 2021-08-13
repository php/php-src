--TEST--
Bug #79580: date_create_from_format misses leap year
--FILE--
<?php
$tz = timezone_open( "UTC" );
$values = [ "31 2020", "60 2020", "91 2020", "121 2020", "130 2020" ];

foreach ( $values as $value )
{
	echo "Testing for {$value}: ";
	$dt = DateTime::createFromFormat( 'z Y', $value );
	if (!$dt) {
		echo DateTime::getLastErrors()['errors'][0], "\n";
	} else {
		echo $dt->format('Y-m-d'), "\n";
	}
}
?>
--EXPECT--
Testing for 31 2020: A 'day of year' can only come after a year has been found
Testing for 60 2020: A 'day of year' can only come after a year has been found
Testing for 91 2020: A 'day of year' can only come after a year has been found
Testing for 121 2020: A 'day of year' can only come after a year has been found
Testing for 130 2020: A 'day of year' can only come after a year has been found
