--TEST--
DatePeriod
--FILE--
<?php
date_default_timezone_set('UTC');
$db1 = new DateTimeImmutable( '2008-01-01' );
$db2 = new DateTime( '2008-01-01' );
$de = new DateTime( '2008-03-31' );
$di = DateInterval::createFromDateString( 'first day of next month' );

foreach ( new DatePeriod( $db1, $di, $de ) as $dt )
{
	echo get_class( $dt ), "\n";
	echo $dt->format( "l Y-m-d\n" );
    echo $dt->modify( "3 tuesday" )->format( "l Y-m-d\n" );
	echo $dt->format( "l Y-m-d\n\n" );
}

foreach ( new DatePeriod( $db2, $di, $de ) as $dt )
{
	echo get_class( $dt ), "\n";
	echo $dt->format( "l Y-m-d\n" );
    echo $dt->modify( "3 tuesday" )->format( "l Y-m-d\n" );
	echo $dt->format( "l Y-m-d\n\n" );
}
?>
--EXPECT--
DateTimeImmutable
Tuesday 2008-01-01
Tuesday 2008-01-15
Tuesday 2008-01-01

DateTimeImmutable
Friday 2008-02-01
Tuesday 2008-02-19
Friday 2008-02-01

DateTimeImmutable
Saturday 2008-03-01
Tuesday 2008-03-18
Saturday 2008-03-01

DateTime
Tuesday 2008-01-01
Tuesday 2008-01-15
Tuesday 2008-01-15

DateTime
Friday 2008-02-01
Tuesday 2008-02-19
Tuesday 2008-02-19

DateTime
Saturday 2008-03-01
Tuesday 2008-03-18
Tuesday 2008-03-18
