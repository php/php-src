--TEST--
Fractions with DateTime objects
--INI--
date.timezone=UTC
--FILE--
<?php
/* This will go wrong, once in a million times */
$ms = date_create()->format('u');
echo ($ms = 0) ? "microseconds = false\n" : "microseconds = true\n";

/* Normal creation */
echo date_create( "2016-10-03 12:47:18.819313" )->format( "Y-m-d H:i:s.u" ), "\n\n";

/* With modifications */
$dt = new DateTimeImmutable( "2016-10-03 12:47:18.819210" );
echo $dt->modify( "+1 day" )->format( "Y-m-d H:i:s.u" ), "\n";

$dt = new DateTimeImmutable( "2016-10-03 12:47:18.081921" );
echo $dt->modify( "-3 months" )->format( "Y-m-d H:i:s.u" ), "\n";

echo "\n";

/* These should reset the time (and hence fraction) to 0 */
$dt = new DateTimeImmutable( "2016-10-03 12:47:18.081921" );
echo $dt->modify( "yesterday" )->format( "Y-m-d H:i:s.u" ), "\n";

$dt = new DateTimeImmutable( "2016-10-03 12:47:18.081921" );
echo $dt->modify( "noon" )->format( "Y-m-d H:i:s.u" ), "\n";

$dt = new DateTimeImmutable( "2016-10-03 12:47:18.081921" );
echo $dt->modify( "10 weekday" )->format( "Y-m-d H:i:s.u" ), "\n";

/* Interval containing fractions */

$dt1 = new DateTimeImmutable( "2016-10-03 13:20:07.103123" );
$dt2 = new DateTimeImmutable( "2016-10-03 13:20:07.481312" );
$diff = $dt1->diff( $dt2 );

var_dump( $diff );

$dt0 = $dt1->sub( $diff );
$dt3 = $dt2->add( $diff );
$dt4 = $dt3->add( $diff );

echo $dt0->format( "Y-m-d H:i:s.u" ), "\n";
echo $dt1->format( "Y-m-d H:i:s.u" ), "\n";
echo $dt2->format( "Y-m-d H:i:s.u" ), "\n";
echo $dt3->format( "Y-m-d H:i:s.u" ), "\n";
echo $dt4->format( "Y-m-d H:i:s.u" ), "\n";
?>
--EXPECTF--
microseconds = true
2016-10-03 12:47:18.819313

2016-10-04 12:47:18.819210
2016-07-03 12:47:18.081921

2016-10-02 00:00:00.000000
2016-10-03 12:00:00.000000
2016-10-17 12:47:18.081921
object(DateInterval)#%d (16) {
  ["y"]=>
  int(0)
  ["m"]=>
  int(0)
  ["d"]=>
  int(0)
  ["h"]=>
  int(0)
  ["i"]=>
  int(0)
  ["s"]=>
  int(0)
  ["f"]=>
  float(0.378189)
  ["weekday"]=>
  int(0)
  ["weekday_behavior"]=>
  int(0)
  ["first_last_day_of"]=>
  int(0)
  ["invert"]=>
  int(0)
  ["days"]=>
  int(0)
  ["special_type"]=>
  int(0)
  ["special_amount"]=>
  int(0)
  ["have_weekday_relative"]=>
  int(0)
  ["have_special_relative"]=>
  int(0)
}
2016-10-03 13:20:06.724934
2016-10-03 13:20:07.103123
2016-10-03 13:20:07.481312
2016-10-03 13:20:07.859501
2016-10-03 13:20:08.237690
