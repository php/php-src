--TEST--
Bug #63740 (strtotime seems to use both sunday and monday as start of week)
--FILE--
<?php
$dates = [
	'2015-07-04',
	'2015-07-05',
	'2015-07-06',
	'2015-07-07',
	'2015-07-08',
	'2015-07-09',
	'2015-07-10',
	'2015-07-11',
	'2015-07-12',
	'2015-07-13',
	'2015-07-14',
];

foreach ( $dates as $date )
{
	$dt = new DateTimeImmutable( "$date 00:00 UTC" );

	echo $dt->format( "D Y-m-d H:i" ), " → ";

	$dtn = $dt->modify( "this week" );

	echo $dtn->format( "D Y-m-d H:i" ), "\n";
}
?>
--EXPECT--
Sat 2015-07-04 00:00 → Mon 2015-06-29 00:00
Sun 2015-07-05 00:00 → Mon 2015-06-29 00:00
Mon 2015-07-06 00:00 → Mon 2015-07-06 00:00
Tue 2015-07-07 00:00 → Mon 2015-07-06 00:00
Wed 2015-07-08 00:00 → Mon 2015-07-06 00:00
Thu 2015-07-09 00:00 → Mon 2015-07-06 00:00
Fri 2015-07-10 00:00 → Mon 2015-07-06 00:00
Sat 2015-07-11 00:00 → Mon 2015-07-06 00:00
Sun 2015-07-12 00:00 → Mon 2015-07-06 00:00
Mon 2015-07-13 00:00 → Mon 2015-07-13 00:00
Tue 2015-07-14 00:00 → Mon 2015-07-13 00:00
