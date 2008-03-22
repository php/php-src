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
--UEXPECTF--	
unicode(3) "Jan"
unicode(7) "January"
unicode(3) "Jan"
unicode(7) "January"
unicode(6) "Shevat"
unicode(0) ""
unicode(3) "Jan"
unicode(7) "January"
unicode(3) "Dec"
unicode(8) "December"
unicode(5) "Tevet"
unicode(0) ""
unicode(0) ""
unicode(0) ""
unicode(0) ""
unicode(0) ""
unicode(0) ""
unicode(0) ""

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
unicode(3) "Dec"
unicode(8) "December"
unicode(3) "Jul"
unicode(4) "July"
unicode(6) "Tishri"
unicode(0) ""
Done
