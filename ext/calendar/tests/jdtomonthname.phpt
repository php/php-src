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
    var_dump(jdmonthname($jd_day,0));
    var_dump(jdmonthname($jd_day,1));
    var_dump(jdmonthname($jd_day,2));
    var_dump(jdmonthname($jd_day,3));
    var_dump(jdmonthname($jd_day,4));
    var_dump(jdmonthname($jd_day,5));
}

echo "Done\n";

?>
--EXPECTF--	
string(3) "Jan"
string(7) "January"
string(3) "Jan"
string(7) "January"
string(6) "Shevat"
string(0) ""
string(3) "Jan"
string(7) "January"
string(3) "Dec"
string(8) "December"
string(5) "Tevet"
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""

Warning: jdmonthname() expects parameter 1 to be long, array given in %s on line %d
bool(false)

Warning: jdmonthname() expects parameter 1 to be long, array given in %s on line %d
bool(false)

Warning: jdmonthname() expects parameter 1 to be long, array given in %s on line %d
bool(false)

Warning: jdmonthname() expects parameter 1 to be long, array given in %s on line %d
bool(false)

Warning: jdmonthname() expects parameter 1 to be long, array given in %s on line %d
bool(false)

Warning: jdmonthname() expects parameter 1 to be long, array given in %s on line %d
bool(false)
string(3) "Dec"
string(8) "December"
string(3) "Jul"
string(4) "July"
string(6) "Tishri"
string(0) ""
Done
