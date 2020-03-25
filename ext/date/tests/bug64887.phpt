--TEST--
Bug #64887: Allow DateTime modification with subsecond items
--INI--
date.timezone=UTC
--FILE--
<?php
$tests = [
    '+1 ms',
    '-2 msec',
    '+3 msecs',
    '-4 millisecond',
    '+5 milliseconds',

    '-6 usec',
    '+7 usecs',
    '-8 microsecond',
    '+9 microseconds',
    '-10 µs',
    '+11 µsec',
    '-12 µsecs',

    '+8 msec -2 µsec',
];

$datetime = new DateTimeImmutable( "2016-10-07 13:25:50" );

foreach ( $tests as $test )
{
    echo $datetime->modify( $test )->format( 'Y-m-d H:i:s.u' ), "\n";
}

?>
--EXPECT--
2016-10-07 13:25:50.001000
2016-10-07 13:25:49.998000
2016-10-07 13:25:50.003000
2016-10-07 13:25:49.996000
2016-10-07 13:25:50.005000
2016-10-07 13:25:49.999994
2016-10-07 13:25:50.000007
2016-10-07 13:25:49.999992
2016-10-07 13:25:50.000009
2016-10-07 13:25:49.999990
2016-10-07 13:25:50.000011
2016-10-07 13:25:49.999988
2016-10-07 13:25:50.007998
