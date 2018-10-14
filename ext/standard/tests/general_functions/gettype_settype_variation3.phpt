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
/* Prototype: string gettype ( mixed $var );
   Description: Returns the type of the PHP variable var

   Prototype: bool settype ( mixed &$var, string $type );
   Description: Set the type of variable var to type
*/

/* Test usage variation of gettype() and settype() functions:
         settype() to float/double type.
   Set type of the data to "float"/"double" and verify using gettype
   Following are performed in the listed sequence:
     get the current type of the variable
     set the type of the variable to float/double type
     dump the variable to see its new data
     get the new type of the variable
*/

/* function to handle catchable errors */
function foo($errno, $errstr, $errfile, $errline) {
//	var_dump($errstr);
   // print error no and error string
   echo "$errno: $errstr\n";
}
//set the error handler, this is required as
// settype() would fail with catachable fatal error
set_error_handler("foo");

$var1 = "another string";
$var2 = array(2,3,4);

// a variable which is unset
$unset_var = 10.5;
unset( $unset_var );

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
  "\xFF",
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

  /* undefined/unset vars */
  $unset_var,
  $undef_var
);

// test conversion to these types
$types = array(
  "float",
  "double"
);

echo "\n*** Testing settype() & gettype() : usage variations ***\n";
foreach ($types as $type) {
  echo "\n-- Setting type of data to $type --\n";
  $inner_loop_count = 1;
  foreach ($var_values as $var) {
    echo "-- Iteration $inner_loop_count --\n"; $inner_loop_count++;

    // get the current data type
    var_dump( gettype($var) );

    // convert it to new type
    var_dump( settype($var, $type) );

    // dump the converted $var
    var_dump( $var );

    // get the new type of the $var
    var_dump( gettype($var) );
  }
}

echo "Done\n";
?>
--EXPECTF--
8: Undefined variable: unset_var
8: Undefined variable: undef_var

*** Testing settype() & gettype() : usage variations ***

-- Setting type of data to float --
-- Iteration 1 --
string(4) "NULL"
bool(true)
float(0)
string(6) "double"
-- Iteration 2 --
string(7) "boolean"
bool(true)
float(0)
string(6) "double"
-- Iteration 3 --
string(7) "boolean"
bool(true)
float(1)
string(6) "double"
-- Iteration 4 --
string(7) "boolean"
bool(true)
float(1)
string(6) "double"
-- Iteration 5 --
string(6) "string"
bool(true)
float(0)
string(6) "double"
-- Iteration 6 --
string(6) "string"
bool(true)
float(0)
string(6) "double"
-- Iteration 7 --
string(6) "string"
bool(true)
float(3)
string(6) "double"
-- Iteration 8 --
string(6) "string"
bool(true)
float(0)
string(6) "double"
-- Iteration 9 --
string(6) "string"
bool(true)
float(0)
string(6) "double"
-- Iteration 10 --
string(6) "string"
bool(true)
float(0)
string(6) "double"
-- Iteration 11 --
string(6) "string"
bool(true)
float(0)
string(6) "double"
-- Iteration 12 --
string(6) "string"
bool(true)
float(10)
string(6) "double"
-- Iteration 13 --
string(6) "string"
bool(true)
float(10)
string(6) "double"
-- Iteration 14 --
string(6) "string"
bool(true)
float(10)
string(6) "double"
-- Iteration 15 --
string(6) "string"
bool(true)
float(10)
string(6) "double"
-- Iteration 16 --
string(6) "string"
bool(true)
float(1)
string(6) "double"
-- Iteration 17 --
string(6) "string"
bool(true)
float(-1)
string(6) "double"
-- Iteration 18 --
string(6) "string"
bool(true)
float(100)
string(6) "double"
-- Iteration 19 --
string(6) "string"
bool(true)
float(1)
string(6) "double"
-- Iteration 20 --
string(6) "string"
bool(true)
float(2.9743947493287E+21)
string(6) "double"
-- Iteration 21 --
string(6) "string"
bool(true)
float(-0.01)
string(6) "double"
-- Iteration 22 --
string(6) "string"
bool(true)
float(1)
string(6) "double"
-- Iteration 23 --
string(6) "string"
bool(true)
float(-1)
string(6) "double"
-- Iteration 24 --
string(6) "string"
bool(true)
float(100)
string(6) "double"
-- Iteration 25 --
string(6) "string"
bool(true)
float(1)
string(6) "double"
-- Iteration 26 --
string(6) "string"
bool(true)
float(2.9743947493287E+21)
string(6) "double"
-- Iteration 27 --
string(6) "string"
bool(true)
float(-0.01)
string(6) "double"
-- Iteration 28 --
string(6) "string"
bool(true)
float(0)
string(6) "double"
-- Iteration 29 --
string(6) "string"
bool(true)
float(0)
string(6) "double"
-- Iteration 30 --
string(6) "string"
bool(true)
float(0)
string(6) "double"
-- Iteration 31 --
string(6) "string"
bool(true)
float(0)
string(6) "double"
-- Iteration 32 --
string(6) "string"
bool(true)
float(123)
string(6) "double"
-- Iteration 33 --
string(6) "string"
bool(true)
float(123)
string(6) "double"
-- Iteration 34 --
string(6) "string"
bool(true)
float(-123)
string(6) "double"
-- Iteration 35 --
string(6) "string"
bool(true)
float(123)
string(6) "double"
-- Iteration 36 --
string(6) "string"
bool(true)
float(-123)
string(6) "double"
-- Iteration 37 --
string(6) "string"
bool(true)
float(123)
string(6) "double"
-- Iteration 38 --
string(6) "string"
bool(true)
float(-0)
string(6) "double"
-- Iteration 39 --
string(6) "string"
bool(true)
float(0)
string(6) "double"
-- Iteration 40 --
string(6) "string"
bool(true)
float(-0)
string(6) "double"
-- Iteration 41 --
string(6) "string"
bool(true)
float(0)
string(6) "double"
-- Iteration 42 --
string(6) "string"
bool(true)
float(0)
string(6) "double"
-- Iteration 43 --
string(5) "array"
bool(true)
float(0)
string(6) "double"
-- Iteration 44 --
string(5) "array"
bool(true)
float(1)
string(6) "double"
-- Iteration 45 --
string(5) "array"
bool(true)
float(1)
string(6) "double"
-- Iteration 46 --
string(5) "array"
bool(true)
float(1)
string(6) "double"
-- Iteration 47 --
string(5) "array"
bool(true)
float(1)
string(6) "double"
-- Iteration 48 --
string(6) "double"
bool(true)
float(-2147483648)
string(6) "double"
-- Iteration 49 --
string(7) "integer"
bool(true)
float(2147483647)
string(6) "double"
-- Iteration 50 --
string(6) "double"
bool(true)
float(2147483649)
string(6) "double"
-- Iteration 51 --
string(6) "double"
bool(true)
float(1232147483649)
string(6) "double"
-- Iteration 52 --
string(7) "integer"
bool(true)
float(85)
string(6) "double"
-- Iteration 53 --
string(6) "double"
bool(true)
float(1058513956921)
string(6) "double"
-- Iteration 54 --
string(7) "integer"
bool(true)
float(-21903)
string(6) "double"
-- Iteration 55 --
string(7) "integer"
bool(true)
float(365)
string(6) "double"
-- Iteration 56 --
string(7) "integer"
bool(true)
float(-365)
string(6) "double"
-- Iteration 57 --
string(6) "double"
bool(true)
float(80561044571754)
string(6) "double"
-- Iteration 58 --
string(6) "double"
bool(true)
float(100000)
string(6) "double"
-- Iteration 59 --
string(6) "double"
bool(true)
float(-100000)
string(6) "double"
-- Iteration 60 --
string(6) "double"
bool(true)
float(100000)
string(6) "double"
-- Iteration 61 --
string(6) "double"
bool(true)
float(-100000)
string(6) "double"
-- Iteration 62 --
string(6) "double"
bool(true)
float(-1.5)
string(6) "double"
-- Iteration 63 --
string(6) "double"
bool(true)
float(0.5)
string(6) "double"
-- Iteration 64 --
string(6) "double"
bool(true)
float(-0.5)
string(6) "double"
-- Iteration 65 --
string(6) "double"
bool(true)
float(500000)
string(6) "double"
-- Iteration 66 --
string(6) "double"
bool(true)
float(-500000)
string(6) "double"
-- Iteration 67 --
string(6) "double"
bool(true)
float(-5.0E-7)
string(6) "double"
-- Iteration 68 --
string(6) "double"
bool(true)
float(500000)
string(6) "double"
-- Iteration 69 --
string(6) "double"
bool(true)
float(-500000)
string(6) "double"
-- Iteration 70 --
string(6) "double"
bool(true)
float(512000)
string(6) "double"
-- Iteration 71 --
string(6) "double"
bool(true)
float(-512000)
string(6) "double"
-- Iteration 72 --
string(6) "double"
bool(true)
float(5.12E-7)
string(6) "double"
-- Iteration 73 --
string(6) "double"
bool(true)
float(5.12E-7)
string(6) "double"
-- Iteration 74 --
string(6) "double"
bool(true)
float(512000)
string(6) "double"
-- Iteration 75 --
string(6) "double"
bool(true)
float(-512000)
string(6) "double"
-- Iteration 76 --
string(6) "object"
8: Object of class point could not be converted to float
bool(true)
float(1)
string(6) "double"
-- Iteration 77 --
string(6) "object"
8: Object of class point could not be converted to float
bool(true)
float(1)
string(6) "double"
-- Iteration 78 --
string(6) "object"
8: Object of class point could not be converted to float
bool(true)
float(1)
string(6) "double"
-- Iteration 79 --
string(4) "NULL"
bool(true)
float(0)
string(6) "double"
-- Iteration 80 --
string(4) "NULL"
bool(true)
float(0)
string(6) "double"

-- Setting type of data to double --
-- Iteration 1 --
string(4) "NULL"
bool(true)
float(0)
string(6) "double"
-- Iteration 2 --
string(7) "boolean"
bool(true)
float(0)
string(6) "double"
-- Iteration 3 --
string(7) "boolean"
bool(true)
float(1)
string(6) "double"
-- Iteration 4 --
string(7) "boolean"
bool(true)
float(1)
string(6) "double"
-- Iteration 5 --
string(6) "string"
bool(true)
float(0)
string(6) "double"
-- Iteration 6 --
string(6) "string"
bool(true)
float(0)
string(6) "double"
-- Iteration 7 --
string(6) "string"
bool(true)
float(3)
string(6) "double"
-- Iteration 8 --
string(6) "string"
bool(true)
float(0)
string(6) "double"
-- Iteration 9 --
string(6) "string"
bool(true)
float(0)
string(6) "double"
-- Iteration 10 --
string(6) "string"
bool(true)
float(0)
string(6) "double"
-- Iteration 11 --
string(6) "string"
bool(true)
float(0)
string(6) "double"
-- Iteration 12 --
string(6) "string"
bool(true)
float(10)
string(6) "double"
-- Iteration 13 --
string(6) "string"
bool(true)
float(10)
string(6) "double"
-- Iteration 14 --
string(6) "string"
bool(true)
float(10)
string(6) "double"
-- Iteration 15 --
string(6) "string"
bool(true)
float(10)
string(6) "double"
-- Iteration 16 --
string(6) "string"
bool(true)
float(1)
string(6) "double"
-- Iteration 17 --
string(6) "string"
bool(true)
float(-1)
string(6) "double"
-- Iteration 18 --
string(6) "string"
bool(true)
float(100)
string(6) "double"
-- Iteration 19 --
string(6) "string"
bool(true)
float(1)
string(6) "double"
-- Iteration 20 --
string(6) "string"
bool(true)
float(2.9743947493287E+21)
string(6) "double"
-- Iteration 21 --
string(6) "string"
bool(true)
float(-0.01)
string(6) "double"
-- Iteration 22 --
string(6) "string"
bool(true)
float(1)
string(6) "double"
-- Iteration 23 --
string(6) "string"
bool(true)
float(-1)
string(6) "double"
-- Iteration 24 --
string(6) "string"
bool(true)
float(100)
string(6) "double"
-- Iteration 25 --
string(6) "string"
bool(true)
float(1)
string(6) "double"
-- Iteration 26 --
string(6) "string"
bool(true)
float(2.9743947493287E+21)
string(6) "double"
-- Iteration 27 --
string(6) "string"
bool(true)
float(-0.01)
string(6) "double"
-- Iteration 28 --
string(6) "string"
bool(true)
float(0)
string(6) "double"
-- Iteration 29 --
string(6) "string"
bool(true)
float(0)
string(6) "double"
-- Iteration 30 --
string(6) "string"
bool(true)
float(0)
string(6) "double"
-- Iteration 31 --
string(6) "string"
bool(true)
float(0)
string(6) "double"
-- Iteration 32 --
string(6) "string"
bool(true)
float(123)
string(6) "double"
-- Iteration 33 --
string(6) "string"
bool(true)
float(123)
string(6) "double"
-- Iteration 34 --
string(6) "string"
bool(true)
float(-123)
string(6) "double"
-- Iteration 35 --
string(6) "string"
bool(true)
float(123)
string(6) "double"
-- Iteration 36 --
string(6) "string"
bool(true)
float(-123)
string(6) "double"
-- Iteration 37 --
string(6) "string"
bool(true)
float(123)
string(6) "double"
-- Iteration 38 --
string(6) "string"
bool(true)
float(-0)
string(6) "double"
-- Iteration 39 --
string(6) "string"
bool(true)
float(0)
string(6) "double"
-- Iteration 40 --
string(6) "string"
bool(true)
float(-0)
string(6) "double"
-- Iteration 41 --
string(6) "string"
bool(true)
float(0)
string(6) "double"
-- Iteration 42 --
string(6) "string"
bool(true)
float(0)
string(6) "double"
-- Iteration 43 --
string(5) "array"
bool(true)
float(0)
string(6) "double"
-- Iteration 44 --
string(5) "array"
bool(true)
float(1)
string(6) "double"
-- Iteration 45 --
string(5) "array"
bool(true)
float(1)
string(6) "double"
-- Iteration 46 --
string(5) "array"
bool(true)
float(1)
string(6) "double"
-- Iteration 47 --
string(5) "array"
bool(true)
float(1)
string(6) "double"
-- Iteration 48 --
string(6) "double"
bool(true)
float(-2147483648)
string(6) "double"
-- Iteration 49 --
string(7) "integer"
bool(true)
float(2147483647)
string(6) "double"
-- Iteration 50 --
string(6) "double"
bool(true)
float(2147483649)
string(6) "double"
-- Iteration 51 --
string(6) "double"
bool(true)
float(1232147483649)
string(6) "double"
-- Iteration 52 --
string(7) "integer"
bool(true)
float(85)
string(6) "double"
-- Iteration 53 --
string(6) "double"
bool(true)
float(1058513956921)
string(6) "double"
-- Iteration 54 --
string(7) "integer"
bool(true)
float(-21903)
string(6) "double"
-- Iteration 55 --
string(7) "integer"
bool(true)
float(365)
string(6) "double"
-- Iteration 56 --
string(7) "integer"
bool(true)
float(-365)
string(6) "double"
-- Iteration 57 --
string(6) "double"
bool(true)
float(80561044571754)
string(6) "double"
-- Iteration 58 --
string(6) "double"
bool(true)
float(100000)
string(6) "double"
-- Iteration 59 --
string(6) "double"
bool(true)
float(-100000)
string(6) "double"
-- Iteration 60 --
string(6) "double"
bool(true)
float(100000)
string(6) "double"
-- Iteration 61 --
string(6) "double"
bool(true)
float(-100000)
string(6) "double"
-- Iteration 62 --
string(6) "double"
bool(true)
float(-1.5)
string(6) "double"
-- Iteration 63 --
string(6) "double"
bool(true)
float(0.5)
string(6) "double"
-- Iteration 64 --
string(6) "double"
bool(true)
float(-0.5)
string(6) "double"
-- Iteration 65 --
string(6) "double"
bool(true)
float(500000)
string(6) "double"
-- Iteration 66 --
string(6) "double"
bool(true)
float(-500000)
string(6) "double"
-- Iteration 67 --
string(6) "double"
bool(true)
float(-5.0E-7)
string(6) "double"
-- Iteration 68 --
string(6) "double"
bool(true)
float(500000)
string(6) "double"
-- Iteration 69 --
string(6) "double"
bool(true)
float(-500000)
string(6) "double"
-- Iteration 70 --
string(6) "double"
bool(true)
float(512000)
string(6) "double"
-- Iteration 71 --
string(6) "double"
bool(true)
float(-512000)
string(6) "double"
-- Iteration 72 --
string(6) "double"
bool(true)
float(5.12E-7)
string(6) "double"
-- Iteration 73 --
string(6) "double"
bool(true)
float(5.12E-7)
string(6) "double"
-- Iteration 74 --
string(6) "double"
bool(true)
float(512000)
string(6) "double"
-- Iteration 75 --
string(6) "double"
bool(true)
float(-512000)
string(6) "double"
-- Iteration 76 --
string(6) "object"
8: Object of class point could not be converted to float
bool(true)
float(1)
string(6) "double"
-- Iteration 77 --
string(6) "object"
8: Object of class point could not be converted to float
bool(true)
float(1)
string(6) "double"
-- Iteration 78 --
string(6) "object"
8: Object of class point could not be converted to float
bool(true)
float(1)
string(6) "double"
-- Iteration 79 --
string(4) "NULL"
bool(true)
float(0)
string(6) "double"
-- Iteration 80 --
string(4) "NULL"
bool(true)
float(0)
string(6) "double"
Done
