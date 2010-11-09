--TEST--
SplFileObject::fputcsv(): error conditions
--FILE--
<?php
$fo = new SplFileObject('SplFileObject::fputcsv.csv', 'w');

echo "*** Testing error conditions ***\n";
// zero argument
echo "-- Testing fputcsv() with zero argument --\n";
var_dump( $fo->fputcsv() );

// more than expected no. of args
echo "-- Testing fputcsv() with more than expected number of arguments --\n";
$fields = array("fld1", "fld2");
$delim = ";";
$enclosure ="\"";
var_dump( $fo->fputcsv($fields, $delim, $enclosure, $fo) );

echo "Done\n";
--EXPECTF--
*** Testing error conditions ***
-- Testing fputcsv() with zero argument --

Warning: SplFileObject::fputcsv() expects at least 1 parameter, 0 given in %s on line %d
NULL
-- Testing fputcsv() with more than expected number of arguments --

Warning: SplFileObject::fputcsv() expects at most 3 parameters, 4 given in %s on line %d
NULL
Done
