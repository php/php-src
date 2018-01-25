--TEST--
Bug #51096 (Test for "first day" vs "first day of")
--FILE--
<?php
$tests = array(
	'first day',
	'last day',
	'next month',
	'first day next month',
	'last day next month',
	'first day of next month',
	'last day of next month'
);

foreach ( $tests as $test )
{
	$result = date_parse( $test );
	$rel = $result['relative'];
	echo $test, "\n- month: ", $rel['month'], '; day: ', $rel['day'],
		 '; first-day-of: ', isset( $rel['first_day_of_month'] ) ? 'true' : 'false',
		 '; last-day-of: ', isset( $rel['last_day_of_month'] ) ? 'true' : 'false', "\n";
	$date = new DateTime( '2010-03-06 15:21 UTC' );
	echo '- ', $date->format( DateTime::ISO8601 );
	$date->modify( $test );
	echo ' -> ', $date->format( DateTime::ISO8601 ), "\n\n";
}
?>
--EXPECT--
first day
- month: 0; day: 1; first-day-of: false; last-day-of: false
- 2010-03-06T15:21:00+0000 -> 2010-03-07T15:21:00+0000

last day
- month: 0; day: -1; first-day-of: false; last-day-of: false
- 2010-03-06T15:21:00+0000 -> 2010-03-05T15:21:00+0000

next month
- month: 1; day: 0; first-day-of: false; last-day-of: false
- 2010-03-06T15:21:00+0000 -> 2010-04-06T15:21:00+0000

first day next month
- month: 1; day: 1; first-day-of: false; last-day-of: false
- 2010-03-06T15:21:00+0000 -> 2010-04-07T15:21:00+0000

last day next month
- month: 1; day: -1; first-day-of: false; last-day-of: false
- 2010-03-06T15:21:00+0000 -> 2010-04-05T15:21:00+0000

first day of next month
- month: 1; day: 0; first-day-of: true; last-day-of: false
- 2010-03-06T15:21:00+0000 -> 2010-04-01T15:21:00+0000

last day of next month
- month: 1; day: 0; first-day-of: false; last-day-of: true
- 2010-03-06T15:21:00+0000 -> 2010-04-30T15:21:00+0000
