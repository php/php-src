--TEST--
DatePeriod
--FILE--
<?php
date_default_timezone_set('UTC');
$db = new DateTime( '2008-01-01' );
$de = new DateTime( '2008-12-31' );
$di = DateInterval::createFromDateString( 'first day of next month' );

foreach ( new DatePeriod( $db, $di, $de ) as $dt )
{
    echo $dt->modify( "3 tuesday" )->format( "l Y-m-d\n" );
}
?>

<?php
$db = new DateTime( '2007-12-31' );
$de = new DateTime( '2009-12-31 23:59:59' );
$di = DateInterval::createFromDateString( 'last thursday of next month' );

foreach ( new DatePeriod( $db, $di, $de, DatePeriod::EXCLUDE_START_DATE ) as $dt )
{
    echo $dt->format( "l Y-m-d H:i:s\n" );
}
?>
--EXPECT--
Tuesday 2008-01-15
Tuesday 2008-02-19
Tuesday 2008-03-18
Tuesday 2008-04-15
Tuesday 2008-05-20
Tuesday 2008-06-17
Tuesday 2008-07-15
Tuesday 2008-08-19
Tuesday 2008-09-16
Tuesday 2008-10-21
Tuesday 2008-11-18
Tuesday 2008-12-16

Thursday 2008-01-31 00:00:00
Thursday 2008-02-28 00:00:00
Thursday 2008-03-27 00:00:00
Thursday 2008-04-24 00:00:00
Thursday 2008-05-29 00:00:00
Thursday 2008-06-26 00:00:00
Thursday 2008-07-31 00:00:00
Thursday 2008-08-28 00:00:00
Thursday 2008-09-25 00:00:00
Thursday 2008-10-30 00:00:00
Thursday 2008-11-27 00:00:00
Thursday 2008-12-25 00:00:00
Thursday 2009-01-29 00:00:00
Thursday 2009-02-26 00:00:00
Thursday 2009-03-26 00:00:00
Thursday 2009-04-30 00:00:00
Thursday 2009-05-28 00:00:00
Thursday 2009-06-25 00:00:00
Thursday 2009-07-30 00:00:00
Thursday 2009-08-27 00:00:00
Thursday 2009-09-24 00:00:00
Thursday 2009-10-29 00:00:00
Thursday 2009-11-26 00:00:00
Thursday 2009-12-31 00:00:00
