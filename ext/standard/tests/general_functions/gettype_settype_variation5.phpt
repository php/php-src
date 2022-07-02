--TEST--
Test gettype() & settype() functions : usage variations
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only");
?>
--INI--
precision=14
--FILE--
<?php
/* Test usage variation of gettype() and settype() functions:
         settype() to resource type.
   Set type of the data to "resource" and verify using gettype
   Following are performed in the listed sequence:
     get the current type of the variable
     set the type of the variable to resource type
     dump the variable to see its new data
     get the new type of the variable
*/
$var1 = "another string";
$var2 = array(2,3,4);

class point
{
  var $x;
  var $y;

  function __construct($x, $y) {
     $this->x = $x;
     $this->y = $y;
  }

  function __toString() {
     return "ObjectPoint";
  }
}

$var_values = array (
  /* nulls */
  null,

  /* boolean */
  FALSE,
  TRUE,
  true,

  /* strings */
  "\$",
  "\x66",
  "\0123",
  "",
  '',
  " ",
  ' ',
  /* numerics in the form of string */
  '10',
  "10",
  "10string",
  '10string',
  "1",
  "-1",
  "1e2",
  " 1",
  "2974394749328742328432",
  "-1e-2",
  '1',
  '-1',
  '1e2',
  ' 1',
  '2974394749328742328432',
  '-1e-2',
  "0xff",
  '0x55',
  '0XA55',
  '0X123',
  "0123",
  '0123',
  "-0123",
  "+0123",
  '-0123',
  '+0123',
  "-0x80001", // invalid numerics as its prefix with sign or have decimal points
  "+0x80001",
  "-0x80001.5",
  "0x80001.5",
  "@$%#$%^$%^&^",

  /* arrays */
  array(),
  array(NULL),
  array(1,2,3,4),
  array(1 => "one", 2 => "two", "3" => "three", "four" => 4),
  array(1.5, 2.4, 6.5e6),

  /* integers */
  -2147483648, // max -ne int value
  2147483647,
  2147483649,
  1232147483649,
  0x55,
  0xF674593039, // a hex value > than max int
  -0X558F,
  0555,
  -0555,
  02224242434343152, // an octal value > than max int

  /* floats */
  1e5,
  -1e5,
  1E5,
  -1E5,
  -1.5,
  .5,
  -.5,
  .5e6,
  -.5e6,
  -.5e-6,
  .5e+6,
  -.5e+6,
  .512E6,
  -.512E6,
  .512E-6,
  +.512E-6,
  .512E+6,
  -.512E+6,

  new point(NULL, NULL),
  new point(2.5, 40.5),
  new point(0, 0),
);

/* test conversion to resource type */
$type = "resource";

echo "\n*** Testing gettype() & settype() functions : usage variations ***\n";
echo "\n-- Setting type of data to $type --\n";
$loop_count = 1;
foreach ($var_values as $var) {
  echo "-- Iteration $loop_count --\n"; $loop_count++;

  // get the current data type
  var_dump( gettype($var) );

  // convert it to resource
  try {
    var_dump(settype($var, $type));
  } catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
  }

  // dump the converted data
  var_dump($var);

  // check the new type after conversion
  var_dump(gettype($var));
}

echo "Done\n";
?>
--EXPECT--
*** Testing gettype() & settype() functions : usage variations ***

-- Setting type of data to resource --
-- Iteration 1 --
string(4) "NULL"
Cannot convert to resource type
NULL
string(4) "NULL"
-- Iteration 2 --
string(7) "boolean"
Cannot convert to resource type
bool(false)
string(7) "boolean"
-- Iteration 3 --
string(7) "boolean"
Cannot convert to resource type
bool(true)
string(7) "boolean"
-- Iteration 4 --
string(7) "boolean"
Cannot convert to resource type
bool(true)
string(7) "boolean"
-- Iteration 5 --
string(6) "string"
Cannot convert to resource type
string(1) "$"
string(6) "string"
-- Iteration 6 --
string(6) "string"
Cannot convert to resource type
string(1) "f"
string(6) "string"
-- Iteration 7 --
string(6) "string"
Cannot convert to resource type
string(2) "
3"
string(6) "string"
-- Iteration 8 --
string(6) "string"
Cannot convert to resource type
string(0) ""
string(6) "string"
-- Iteration 9 --
string(6) "string"
Cannot convert to resource type
string(0) ""
string(6) "string"
-- Iteration 10 --
string(6) "string"
Cannot convert to resource type
string(1) " "
string(6) "string"
-- Iteration 11 --
string(6) "string"
Cannot convert to resource type
string(1) " "
string(6) "string"
-- Iteration 12 --
string(6) "string"
Cannot convert to resource type
string(2) "10"
string(6) "string"
-- Iteration 13 --
string(6) "string"
Cannot convert to resource type
string(2) "10"
string(6) "string"
-- Iteration 14 --
string(6) "string"
Cannot convert to resource type
string(8) "10string"
string(6) "string"
-- Iteration 15 --
string(6) "string"
Cannot convert to resource type
string(8) "10string"
string(6) "string"
-- Iteration 16 --
string(6) "string"
Cannot convert to resource type
string(1) "1"
string(6) "string"
-- Iteration 17 --
string(6) "string"
Cannot convert to resource type
string(2) "-1"
string(6) "string"
-- Iteration 18 --
string(6) "string"
Cannot convert to resource type
string(3) "1e2"
string(6) "string"
-- Iteration 19 --
string(6) "string"
Cannot convert to resource type
string(2) " 1"
string(6) "string"
-- Iteration 20 --
string(6) "string"
Cannot convert to resource type
string(22) "2974394749328742328432"
string(6) "string"
-- Iteration 21 --
string(6) "string"
Cannot convert to resource type
string(5) "-1e-2"
string(6) "string"
-- Iteration 22 --
string(6) "string"
Cannot convert to resource type
string(1) "1"
string(6) "string"
-- Iteration 23 --
string(6) "string"
Cannot convert to resource type
string(2) "-1"
string(6) "string"
-- Iteration 24 --
string(6) "string"
Cannot convert to resource type
string(3) "1e2"
string(6) "string"
-- Iteration 25 --
string(6) "string"
Cannot convert to resource type
string(2) " 1"
string(6) "string"
-- Iteration 26 --
string(6) "string"
Cannot convert to resource type
string(22) "2974394749328742328432"
string(6) "string"
-- Iteration 27 --
string(6) "string"
Cannot convert to resource type
string(5) "-1e-2"
string(6) "string"
-- Iteration 28 --
string(6) "string"
Cannot convert to resource type
string(4) "0xff"
string(6) "string"
-- Iteration 29 --
string(6) "string"
Cannot convert to resource type
string(4) "0x55"
string(6) "string"
-- Iteration 30 --
string(6) "string"
Cannot convert to resource type
string(5) "0XA55"
string(6) "string"
-- Iteration 31 --
string(6) "string"
Cannot convert to resource type
string(5) "0X123"
string(6) "string"
-- Iteration 32 --
string(6) "string"
Cannot convert to resource type
string(4) "0123"
string(6) "string"
-- Iteration 33 --
string(6) "string"
Cannot convert to resource type
string(4) "0123"
string(6) "string"
-- Iteration 34 --
string(6) "string"
Cannot convert to resource type
string(5) "-0123"
string(6) "string"
-- Iteration 35 --
string(6) "string"
Cannot convert to resource type
string(5) "+0123"
string(6) "string"
-- Iteration 36 --
string(6) "string"
Cannot convert to resource type
string(5) "-0123"
string(6) "string"
-- Iteration 37 --
string(6) "string"
Cannot convert to resource type
string(5) "+0123"
string(6) "string"
-- Iteration 38 --
string(6) "string"
Cannot convert to resource type
string(8) "-0x80001"
string(6) "string"
-- Iteration 39 --
string(6) "string"
Cannot convert to resource type
string(8) "+0x80001"
string(6) "string"
-- Iteration 40 --
string(6) "string"
Cannot convert to resource type
string(10) "-0x80001.5"
string(6) "string"
-- Iteration 41 --
string(6) "string"
Cannot convert to resource type
string(9) "0x80001.5"
string(6) "string"
-- Iteration 42 --
string(6) "string"
Cannot convert to resource type
string(12) "@$%#$%^$%^&^"
string(6) "string"
-- Iteration 43 --
string(5) "array"
Cannot convert to resource type
array(0) {
}
string(5) "array"
-- Iteration 44 --
string(5) "array"
Cannot convert to resource type
array(1) {
  [0]=>
  NULL
}
string(5) "array"
-- Iteration 45 --
string(5) "array"
Cannot convert to resource type
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
}
string(5) "array"
-- Iteration 46 --
string(5) "array"
Cannot convert to resource type
array(4) {
  [1]=>
  string(3) "one"
  [2]=>
  string(3) "two"
  [3]=>
  string(5) "three"
  ["four"]=>
  int(4)
}
string(5) "array"
-- Iteration 47 --
string(5) "array"
Cannot convert to resource type
array(3) {
  [0]=>
  float(1.5)
  [1]=>
  float(2.4)
  [2]=>
  float(6500000)
}
string(5) "array"
-- Iteration 48 --
string(6) "double"
Cannot convert to resource type
float(-2147483648)
string(6) "double"
-- Iteration 49 --
string(7) "integer"
Cannot convert to resource type
int(2147483647)
string(7) "integer"
-- Iteration 50 --
string(6) "double"
Cannot convert to resource type
float(2147483649)
string(6) "double"
-- Iteration 51 --
string(6) "double"
Cannot convert to resource type
float(1232147483649)
string(6) "double"
-- Iteration 52 --
string(7) "integer"
Cannot convert to resource type
int(85)
string(7) "integer"
-- Iteration 53 --
string(6) "double"
Cannot convert to resource type
float(1058513956921)
string(6) "double"
-- Iteration 54 --
string(7) "integer"
Cannot convert to resource type
int(-21903)
string(7) "integer"
-- Iteration 55 --
string(7) "integer"
Cannot convert to resource type
int(365)
string(7) "integer"
-- Iteration 56 --
string(7) "integer"
Cannot convert to resource type
int(-365)
string(7) "integer"
-- Iteration 57 --
string(6) "double"
Cannot convert to resource type
float(80561044571754)
string(6) "double"
-- Iteration 58 --
string(6) "double"
Cannot convert to resource type
float(100000)
string(6) "double"
-- Iteration 59 --
string(6) "double"
Cannot convert to resource type
float(-100000)
string(6) "double"
-- Iteration 60 --
string(6) "double"
Cannot convert to resource type
float(100000)
string(6) "double"
-- Iteration 61 --
string(6) "double"
Cannot convert to resource type
float(-100000)
string(6) "double"
-- Iteration 62 --
string(6) "double"
Cannot convert to resource type
float(-1.5)
string(6) "double"
-- Iteration 63 --
string(6) "double"
Cannot convert to resource type
float(0.5)
string(6) "double"
-- Iteration 64 --
string(6) "double"
Cannot convert to resource type
float(-0.5)
string(6) "double"
-- Iteration 65 --
string(6) "double"
Cannot convert to resource type
float(500000)
string(6) "double"
-- Iteration 66 --
string(6) "double"
Cannot convert to resource type
float(-500000)
string(6) "double"
-- Iteration 67 --
string(6) "double"
Cannot convert to resource type
float(-5.0E-7)
string(6) "double"
-- Iteration 68 --
string(6) "double"
Cannot convert to resource type
float(500000)
string(6) "double"
-- Iteration 69 --
string(6) "double"
Cannot convert to resource type
float(-500000)
string(6) "double"
-- Iteration 70 --
string(6) "double"
Cannot convert to resource type
float(512000)
string(6) "double"
-- Iteration 71 --
string(6) "double"
Cannot convert to resource type
float(-512000)
string(6) "double"
-- Iteration 72 --
string(6) "double"
Cannot convert to resource type
float(5.12E-7)
string(6) "double"
-- Iteration 73 --
string(6) "double"
Cannot convert to resource type
float(5.12E-7)
string(6) "double"
-- Iteration 74 --
string(6) "double"
Cannot convert to resource type
float(512000)
string(6) "double"
-- Iteration 75 --
string(6) "double"
Cannot convert to resource type
float(-512000)
string(6) "double"
-- Iteration 76 --
string(6) "object"
Cannot convert to resource type
object(point)#1 (2) {
  ["x"]=>
  NULL
  ["y"]=>
  NULL
}
string(6) "object"
-- Iteration 77 --
string(6) "object"
Cannot convert to resource type
object(point)#2 (2) {
  ["x"]=>
  float(2.5)
  ["y"]=>
  float(40.5)
}
string(6) "object"
-- Iteration 78 --
string(6) "object"
Cannot convert to resource type
object(point)#3 (2) {
  ["x"]=>
  int(0)
  ["y"]=>
  int(0)
}
string(6) "object"
Done
