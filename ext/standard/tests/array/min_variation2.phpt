--TEST--
Test variations in usage of min()
--FILE--
<?php
/*
 * Function is implemented in ext/standard/array.c
*/

echo "\n*** Testing arrays  ***\n";

var_dump(min(array(2,1,2)));
var_dump(min(array(-2,1,2)));
var_dump(min(array(2.1,2.11,2.09)));
var_dump(min(array("", "t", "b")));
var_dump(min(array(false, true, false)));
var_dump(min(array(true, false, true)));
var_dump(min(array(1, true, false, true)));
var_dump(min(array(0, true, false, true)));
var_dump(min(array(0, 1, array(2,3))));
var_dump(min(array(2147483645, 2147483646)));
var_dump(min(array(2147483647, 2147483648)));
var_dump(min(array(2147483646, 2147483648)));
var_dump(min(array(-2147483647, -2147483646)));
var_dump(min(array(-2147483648, -2147483647)));
var_dump(min(array(-2147483649, -2147483647)));

echo "\nDone\n";

?>
--EXPECTF--
*** Testing arrays  ***
int(1)
int(-2)
float(2.09)
string(0) ""
bool(false)
bool(false)
bool(false)
int(0)
int(0)
int(2147483645)
int(2147483647)
int(2147483646)
int(-2147483647)
%s(-2147483648)
%s(-2147483649)

Done
