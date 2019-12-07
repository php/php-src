--TEST--
Bug #44562 (Creating instance of DatePeriod crashes)
--FILE--
<?php
date_default_timezone_set('Europe/Oslo');

try
{
	$dp = new DatePeriod('2D');
}
catch (\ValueError $e )
{
	echo $e->getMessage(), "\n";
}

$begin = new DateTime( "2008-07-20T22:44:53+0200" );
$interval = DateInterval::createFromDateString( "1 day" );

$dp = new DatePeriod( $begin, $interval, 10 );
foreach ( $dp as $d )
{
	var_dump ($d->format( DATE_ISO8601 ) );
}

?>
--EXPECT--
The ISO interval '2D' did not contain a start date.
string(24) "2008-07-20T22:44:53+0200"
string(24) "2008-07-21T22:44:53+0200"
string(24) "2008-07-22T22:44:53+0200"
string(24) "2008-07-23T22:44:53+0200"
string(24) "2008-07-24T22:44:53+0200"
string(24) "2008-07-25T22:44:53+0200"
string(24) "2008-07-26T22:44:53+0200"
string(24) "2008-07-27T22:44:53+0200"
string(24) "2008-07-28T22:44:53+0200"
string(24) "2008-07-29T22:44:53+0200"
string(24) "2008-07-30T22:44:53+0200"
