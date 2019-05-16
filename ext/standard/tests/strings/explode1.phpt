--TEST--
Test explode() function
--INI--
error_reporting=2047
precision=14
--FILE--
<?php
/* Prototype: array explode ( string $delimiter, string $string [, int $limit] );
   Description: Returns an array of strings, each of which is a substring of string
                formed by splitting it on boundaries formed by the string delimiter.
                If limit is set, the returned array will contain a maximum of limit
                elements with the last element containing the rest of string.
*/

echo "*** Testing explode() for basic operations ***\n";
$delimiters = array (
  "",  // len=0
  NULL,
  "abcd",  // string
  0,  // zero
  "0",
  TRUE,  // boolean value
  FALSE,
  -1,  // negative integer
  -11.23,  // double
  4,  // positive integer
  "%",
);
$string = "1234NULL23abcd00000TRUEFALSE-11.234444true-11.24%PHP%ZEND";
/* loop prints an array of strings, each of which is a substring of $string
   formed by splitting it on boundaries formed by the string $delimiter.
 */
$counter = 1;
foreach($delimiters as $delimiter) {
  echo "-- Iteration $counter --\n";
  var_dump( explode($delimiter, $string, -1) );
  var_dump( explode($delimiter, $string, 0) );
  var_dump( explode($delimiter, $string, 1) );
  var_dump( explode($delimiter, $string, 2) );
  $counter++;
}

echo "\n*** Testing explode() with miscelleneous input arguments ***\n";

echo "\n-- Passing positive values of Limit to explode() --\n";
/* LIMIT=2 */
var_dump( explode("::", "mon::tues::wed::thurs::fri::sat::sun", 2) );

/* checking for LIMIT =0,1 */
echo "\n-- Passing limit values 0 and 1 to explode() --\n";
var_dump( explode(":", "Name:Phone:Address:City:State", 0) );
var_dump( explode(":", "Name:Phone:Address:City:State", 1) );

/* to check the maximum limit of string that can be given with limit<=0,
   default size is 50 but increases dynamically */
echo "\n*** Testing explode() for maximum limit of string with Limit = -1 ***\n";
var_dump( explode(":", "1:2:3:4:5:6:7:7:5:6:7:3:4:5:2:8:9:0:5:5:5:5:5:5:5:5:5:5:5:5:55:5:5:5%:%:%:%:5:5:5:%:%:5:5:5:5:5%:%:%:55:1:1", -1) );

echo "\n*** Testing explode() with string variations as input argument ***\n";
/* String with escape characters */
echo "\n-- Testing string with escape characters --\n";
var_dump( explode("\t\n", "1234\t\n5678\n\t9100") );
var_dump( explode("\r", "1234\rabcd\r5678\rrstu") );

/* String with embedded NULL */
echo "\n-- Testing string with embedded NULL --\n";
var_dump( explode("\x00", "abcd\x0n1234\x0005678\x0000efgh\xijkl") );
var_dump( explode("\0", "abcd\0efgh\0ijkl\x00mnop\x000qrst\00uvwx\000yz") );

/* Checking OBJECTS type */
echo "\n*** Testing explode() with objects ***\n";
class string1 {
  public function __toString() {
    return "Object";
  }
}
$obj = new string1;
var_dump( explode("b", $obj) );

echo "Done\n";
?>
--EXPECTF--
*** Testing explode() for basic operations ***
-- Iteration 1 --

Warning: explode(): Empty delimiter in %s on line %d
bool(false)

Warning: explode(): Empty delimiter in %s on line %d
bool(false)

Warning: explode(): Empty delimiter in %s on line %d
bool(false)

Warning: explode(): Empty delimiter in %s on line %d
bool(false)
-- Iteration 2 --

Warning: explode(): Empty delimiter in %s on line %d
bool(false)

Warning: explode(): Empty delimiter in %s on line %d
bool(false)

Warning: explode(): Empty delimiter in %s on line %d
bool(false)

Warning: explode(): Empty delimiter in %s on line %d
bool(false)
-- Iteration 3 --
array(1) {
  [0]=>
  string(10) "1234NULL23"
}
array(1) {
  [0]=>
  string(57) "1234NULL23abcd00000TRUEFALSE-11.234444true-11.24%PHP%ZEND"
}
array(1) {
  [0]=>
  string(57) "1234NULL23abcd00000TRUEFALSE-11.234444true-11.24%PHP%ZEND"
}
array(2) {
  [0]=>
  string(10) "1234NULL23"
  [1]=>
  string(43) "00000TRUEFALSE-11.234444true-11.24%PHP%ZEND"
}
-- Iteration 4 --
array(5) {
  [0]=>
  string(14) "1234NULL23abcd"
  [1]=>
  string(0) ""
  [2]=>
  string(0) ""
  [3]=>
  string(0) ""
  [4]=>
  string(0) ""
}
array(1) {
  [0]=>
  string(57) "1234NULL23abcd00000TRUEFALSE-11.234444true-11.24%PHP%ZEND"
}
array(1) {
  [0]=>
  string(57) "1234NULL23abcd00000TRUEFALSE-11.234444true-11.24%PHP%ZEND"
}
array(2) {
  [0]=>
  string(14) "1234NULL23abcd"
  [1]=>
  string(42) "0000TRUEFALSE-11.234444true-11.24%PHP%ZEND"
}
-- Iteration 5 --
array(5) {
  [0]=>
  string(14) "1234NULL23abcd"
  [1]=>
  string(0) ""
  [2]=>
  string(0) ""
  [3]=>
  string(0) ""
  [4]=>
  string(0) ""
}
array(1) {
  [0]=>
  string(57) "1234NULL23abcd00000TRUEFALSE-11.234444true-11.24%PHP%ZEND"
}
array(1) {
  [0]=>
  string(57) "1234NULL23abcd00000TRUEFALSE-11.234444true-11.24%PHP%ZEND"
}
array(2) {
  [0]=>
  string(14) "1234NULL23abcd"
  [1]=>
  string(42) "0000TRUEFALSE-11.234444true-11.24%PHP%ZEND"
}
-- Iteration 6 --
array(5) {
  [0]=>
  string(0) ""
  [1]=>
  string(28) "234NULL23abcd00000TRUEFALSE-"
  [2]=>
  string(0) ""
  [3]=>
  string(12) ".234444true-"
  [4]=>
  string(0) ""
}
array(1) {
  [0]=>
  string(57) "1234NULL23abcd00000TRUEFALSE-11.234444true-11.24%PHP%ZEND"
}
array(1) {
  [0]=>
  string(57) "1234NULL23abcd00000TRUEFALSE-11.234444true-11.24%PHP%ZEND"
}
array(2) {
  [0]=>
  string(0) ""
  [1]=>
  string(56) "234NULL23abcd00000TRUEFALSE-11.234444true-11.24%PHP%ZEND"
}
-- Iteration 7 --

Warning: explode(): Empty delimiter in %s on line %d
bool(false)

Warning: explode(): Empty delimiter in %s on line %d
bool(false)

Warning: explode(): Empty delimiter in %s on line %d
bool(false)

Warning: explode(): Empty delimiter in %s on line %d
bool(false)
-- Iteration 8 --
array(2) {
  [0]=>
  string(28) "1234NULL23abcd00000TRUEFALSE"
  [1]=>
  string(12) "1.234444true"
}
array(1) {
  [0]=>
  string(57) "1234NULL23abcd00000TRUEFALSE-11.234444true-11.24%PHP%ZEND"
}
array(1) {
  [0]=>
  string(57) "1234NULL23abcd00000TRUEFALSE-11.234444true-11.24%PHP%ZEND"
}
array(2) {
  [0]=>
  string(28) "1234NULL23abcd00000TRUEFALSE"
  [1]=>
  string(27) "1.234444true-11.24%PHP%ZEND"
}
-- Iteration 9 --
array(1) {
  [0]=>
  string(28) "1234NULL23abcd00000TRUEFALSE"
}
array(1) {
  [0]=>
  string(57) "1234NULL23abcd00000TRUEFALSE-11.234444true-11.24%PHP%ZEND"
}
array(1) {
  [0]=>
  string(57) "1234NULL23abcd00000TRUEFALSE-11.234444true-11.24%PHP%ZEND"
}
array(2) {
  [0]=>
  string(28) "1234NULL23abcd00000TRUEFALSE"
  [1]=>
  string(23) "4444true-11.24%PHP%ZEND"
}
-- Iteration 10 --
array(6) {
  [0]=>
  string(3) "123"
  [1]=>
  string(30) "NULL23abcd00000TRUEFALSE-11.23"
  [2]=>
  string(0) ""
  [3]=>
  string(0) ""
  [4]=>
  string(0) ""
  [5]=>
  string(9) "true-11.2"
}
array(1) {
  [0]=>
  string(57) "1234NULL23abcd00000TRUEFALSE-11.234444true-11.24%PHP%ZEND"
}
array(1) {
  [0]=>
  string(57) "1234NULL23abcd00000TRUEFALSE-11.234444true-11.24%PHP%ZEND"
}
array(2) {
  [0]=>
  string(3) "123"
  [1]=>
  string(53) "NULL23abcd00000TRUEFALSE-11.234444true-11.24%PHP%ZEND"
}
-- Iteration 11 --
array(2) {
  [0]=>
  string(48) "1234NULL23abcd00000TRUEFALSE-11.234444true-11.24"
  [1]=>
  string(3) "PHP"
}
array(1) {
  [0]=>
  string(57) "1234NULL23abcd00000TRUEFALSE-11.234444true-11.24%PHP%ZEND"
}
array(1) {
  [0]=>
  string(57) "1234NULL23abcd00000TRUEFALSE-11.234444true-11.24%PHP%ZEND"
}
array(2) {
  [0]=>
  string(48) "1234NULL23abcd00000TRUEFALSE-11.234444true-11.24"
  [1]=>
  string(8) "PHP%ZEND"
}

*** Testing explode() with miscelleneous input arguments ***

-- Passing positive values of Limit to explode() --
array(2) {
  [0]=>
  string(3) "mon"
  [1]=>
  string(31) "tues::wed::thurs::fri::sat::sun"
}

-- Passing limit values 0 and 1 to explode() --
array(1) {
  [0]=>
  string(29) "Name:Phone:Address:City:State"
}
array(1) {
  [0]=>
  string(29) "Name:Phone:Address:City:State"
}

*** Testing explode() for maximum limit of string with Limit = -1 ***
array(51) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "2"
  [2]=>
  string(1) "3"
  [3]=>
  string(1) "4"
  [4]=>
  string(1) "5"
  [5]=>
  string(1) "6"
  [6]=>
  string(1) "7"
  [7]=>
  string(1) "7"
  [8]=>
  string(1) "5"
  [9]=>
  string(1) "6"
  [10]=>
  string(1) "7"
  [11]=>
  string(1) "3"
  [12]=>
  string(1) "4"
  [13]=>
  string(1) "5"
  [14]=>
  string(1) "2"
  [15]=>
  string(1) "8"
  [16]=>
  string(1) "9"
  [17]=>
  string(1) "0"
  [18]=>
  string(1) "5"
  [19]=>
  string(1) "5"
  [20]=>
  string(1) "5"
  [21]=>
  string(1) "5"
  [22]=>
  string(1) "5"
  [23]=>
  string(1) "5"
  [24]=>
  string(1) "5"
  [25]=>
  string(1) "5"
  [26]=>
  string(1) "5"
  [27]=>
  string(1) "5"
  [28]=>
  string(1) "5"
  [29]=>
  string(1) "5"
  [30]=>
  string(2) "55"
  [31]=>
  string(1) "5"
  [32]=>
  string(1) "5"
  [33]=>
  string(2) "5%"
  [34]=>
  string(1) "%"
  [35]=>
  string(1) "%"
  [36]=>
  string(1) "%"
  [37]=>
  string(1) "5"
  [38]=>
  string(1) "5"
  [39]=>
  string(1) "5"
  [40]=>
  string(1) "%"
  [41]=>
  string(1) "%"
  [42]=>
  string(1) "5"
  [43]=>
  string(1) "5"
  [44]=>
  string(1) "5"
  [45]=>
  string(1) "5"
  [46]=>
  string(2) "5%"
  [47]=>
  string(1) "%"
  [48]=>
  string(1) "%"
  [49]=>
  string(2) "55"
  [50]=>
  string(1) "1"
}

*** Testing explode() with string variations as input argument ***

-- Testing string with escape characters --
array(2) {
  [0]=>
  string(4) "1234"
  [1]=>
  string(10) "5678
	9100"
}
array(4) {
  [0]=>
  string(4) "1234"
  [1]=>
  string(4) "abcd"
  [2]=>
  string(4) "5678"
  [3]=>
  string(4) "rstu"
}

-- Testing string with embedded NULL --
array(4) {
  [0]=>
  string(4) "abcd"
  [1]=>
  string(5) "n1234"
  [2]=>
  string(5) "05678"
  [3]=>
  string(12) "00efgh\xijkl"
}
array(7) {
  [0]=>
  string(4) "abcd"
  [1]=>
  string(4) "efgh"
  [2]=>
  string(4) "ijkl"
  [3]=>
  string(4) "mnop"
  [4]=>
  string(5) "0qrst"
  [5]=>
  string(4) "uvwx"
  [6]=>
  string(2) "yz"
}

*** Testing explode() with objects ***
array(2) {
  [0]=>
  string(1) "O"
  [1]=>
  string(4) "ject"
}
Done
