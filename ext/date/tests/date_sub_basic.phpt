--TEST--
Test date_sub() function : basic functionality
--CREDITS--
Felix De Vliegher <felix.devliegher@gmail.com>
--FILE--
<?php
date_default_timezone_set('UTC');
echo "*** Testing date_sub() : basic functionality ***\n";

// Initialise all required variables
$startDate = '2008-01-01 12:25';
$format = 'Y-m-d H:i:s';
$intervals = array(
    'P3Y6M4DT12H30M5S',
    'P0D',
    'P2DT1M',
    'P1Y2MT23H43M150S'
);

$d = new DateTime($startDate);
var_dump( $d->format($format) );

foreach($intervals as $interval) {
    date_sub($d, new DateInterval($interval) );
    var_dump( $d->format($format) );
}

?>
--EXPECT--
*** Testing date_sub() : basic functionality ***
string(19) "2008-01-01 12:25:00"
string(19) "2004-06-26 23:54:55"
string(19) "2004-06-26 23:54:55"
string(19) "2004-06-24 23:53:55"
string(19) "2003-04-24 00:08:25"
