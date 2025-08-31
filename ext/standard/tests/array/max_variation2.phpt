--TEST--
Test variations in usage of max()
--FILE--
<?php
/*
 * Function is implemented in ext/standard/array.c
*/

echo "\n*** Testing arrays  ***\n";

var_dump(max(array(2,1,2)));
var_dump(max(array(-2,1,2)));
var_dump(max(array(2.1,2.11,2.09)));
var_dump(max(array("", "t", "b")));
var_dump(max(array(false, true, false)));
var_dump(max(array(true, false, true)));
var_dump(max(array(1, true, false, true)));
var_dump(max(array(0, true, false, true)));
var_dump(max(array(0, 1, array(2,3))));
var_dump(max(array(2147483645, 2147483646)));
var_dump(max(array(2147483647, 2147483648)));
var_dump(max(array(2147483646, 2147483648)));
var_dump(max(array(-2147483647, -2147483646)));
var_dump(max(array(-2147483648, -2147483647)));
var_dump(max(array(-2147483649, -2147483647)));

echo "\nDone\n";

?>
--EXPECTF--
*** Testing arrays  ***
int(2)
int(2)
float(2.11)
string(1) "t"
bool(true)
bool(true)
int(1)
bool(true)
array(2) {
  [0]=>
  int(2)
  [1]=>
  int(3)
}
int(2147483646)
%s(2147483648)
%s(2147483648)
int(-2147483646)
int(-2147483647)
int(-2147483647)

Done
