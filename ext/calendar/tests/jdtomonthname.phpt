--TEST--
jdtomonthname() test
--SKIPIF--
<?php if (!extension_loaded("calendar")) print "skip"; ?>
--FILE--
<?php

$jd_days = Array(
    2453396,
    2440588,
    -1,
    array(),
    10000000
    );

foreach ($jd_days as $jd_day) {
	echo "=== ", $jd_day, "\n";
    var_dump(jdmonthname($jd_day,0));
    var_dump(jdmonthname($jd_day,1));
    var_dump(jdmonthname($jd_day,2));
    var_dump(jdmonthname($jd_day,3));
    var_dump(jdmonthname($jd_day,4));
    var_dump(jdmonthname($jd_day,5));
	echo "\n";
}

echo "Done\n";

?>
--EXPECTF--
=== 2453396
string(3) "Jan"
string(7) "January"
string(3) "Jan"
string(7) "January"
string(6) "Shevat"
string(0) ""

=== 2440588
string(3) "Jan"
string(7) "January"
string(3) "Dec"
string(8) "December"
string(5) "Tevet"
string(0) ""

=== -1
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""

=== 
Notice: Array to string conversion in %sjdtomonthname.php on line %d
Array

Warning: jdmonthname() expects parameter 1 to be int, array given in %s on line %d
bool(false)

Warning: jdmonthname() expects parameter 1 to be int, array given in %s on line %d
bool(false)

Warning: jdmonthname() expects parameter 1 to be int, array given in %s on line %d
bool(false)

Warning: jdmonthname() expects parameter 1 to be int, array given in %s on line %d
bool(false)

Warning: jdmonthname() expects parameter 1 to be int, array given in %s on line %d
bool(false)

Warning: jdmonthname() expects parameter 1 to be int, array given in %s on line %d
bool(false)

=== 10000000
string(3) "Dec"
string(8) "December"
string(3) "Jul"
string(4) "July"
string(6) "Tishri"
string(0) ""

Done
