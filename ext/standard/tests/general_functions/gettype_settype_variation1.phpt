--TEST--
Test gettype() & settype() functions : usage variatoins
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
         settype() to null type.
   Set type of the data to "null" and verify using gettype
   Following are performed in the listed sequence:
     get the current type of the variable
     set the type of the variable to "null type"
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

  function point($x, $y) {
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
  $undef_var,

  /* binary strings */
  b"10string",
  b'10string',
  b"+0123",
  b'-0123',
  b"0xff",
  b'0x55',
  b'1e2',
  b'2974394749328742328432',
  b"1e2",
  b'10string',
  b"10string"
);

/* test conversion to null type */
$type = "null";

echo "\n*** Testing gettype() & settype() functions : usage variations ***\n";
echo "\n-- Setting type of data to $type --\n";
$loop_count = 1;
foreach ($var_values as $var) {
  echo "-- Iteration $loop_count --\n"; $loop_count++;

  // get the current data type 
  var_dump( gettype($var) );
  
  // convert it to null 
  var_dump( settype($var, $type) );

  // dump the converted data 
  var_dump( $var );
  
  // check the new type after conversion
  var_dump( gettype($var) );
}

echo "Done\n";
?>
--EXPECT--
8: Undefined variable: unset_var
8: Undefined variable: undef_var

*** Testing gettype() & settype() functions : usage variations ***

-- Setting type of data to null --
-- Iteration 1 --
unicode(4) "NULL"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 2 --
unicode(7) "boolean"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 3 --
unicode(7) "boolean"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 4 --
unicode(7) "boolean"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 5 --
unicode(7) "unicode"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 6 --
unicode(7) "unicode"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 7 --
unicode(7) "unicode"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 8 --
unicode(7) "unicode"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 9 --
unicode(7) "unicode"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 10 --
unicode(7) "unicode"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 11 --
unicode(7) "unicode"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 12 --
unicode(7) "unicode"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 13 --
unicode(7) "unicode"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 14 --
unicode(7) "unicode"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 15 --
unicode(7) "unicode"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 16 --
unicode(7) "unicode"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 17 --
unicode(7) "unicode"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 18 --
unicode(7) "unicode"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 19 --
unicode(7) "unicode"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 20 --
unicode(7) "unicode"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 21 --
unicode(7) "unicode"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 22 --
unicode(7) "unicode"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 23 --
unicode(7) "unicode"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 24 --
unicode(7) "unicode"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 25 --
unicode(7) "unicode"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 26 --
unicode(7) "unicode"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 27 --
unicode(7) "unicode"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 28 --
unicode(7) "unicode"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 29 --
unicode(7) "unicode"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 30 --
unicode(7) "unicode"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 31 --
unicode(7) "unicode"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 32 --
unicode(7) "unicode"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 33 --
unicode(7) "unicode"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 34 --
unicode(7) "unicode"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 35 --
unicode(7) "unicode"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 36 --
unicode(7) "unicode"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 37 --
unicode(7) "unicode"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 38 --
unicode(7) "unicode"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 39 --
unicode(7) "unicode"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 40 --
unicode(7) "unicode"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 41 --
unicode(7) "unicode"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 42 --
unicode(7) "unicode"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 43 --
unicode(5) "array"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 44 --
unicode(5) "array"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 45 --
unicode(5) "array"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 46 --
unicode(5) "array"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 47 --
unicode(5) "array"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 48 --
unicode(6) "double"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 49 --
unicode(7) "integer"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 50 --
unicode(6) "double"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 51 --
unicode(6) "double"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 52 --
unicode(7) "integer"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 53 --
unicode(6) "double"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 54 --
unicode(7) "integer"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 55 --
unicode(7) "integer"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 56 --
unicode(7) "integer"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 57 --
unicode(6) "double"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 58 --
unicode(6) "double"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 59 --
unicode(6) "double"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 60 --
unicode(6) "double"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 61 --
unicode(6) "double"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 62 --
unicode(6) "double"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 63 --
unicode(6) "double"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 64 --
unicode(6) "double"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 65 --
unicode(6) "double"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 66 --
unicode(6) "double"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 67 --
unicode(6) "double"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 68 --
unicode(6) "double"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 69 --
unicode(6) "double"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 70 --
unicode(6) "double"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 71 --
unicode(6) "double"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 72 --
unicode(6) "double"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 73 --
unicode(6) "double"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 74 --
unicode(6) "double"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 75 --
unicode(6) "double"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 76 --
unicode(6) "object"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 77 --
unicode(6) "object"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 78 --
unicode(6) "object"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 79 --
unicode(4) "NULL"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 80 --
unicode(4) "NULL"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 81 --
unicode(6) "string"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 82 --
unicode(6) "string"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 83 --
unicode(6) "string"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 84 --
unicode(6) "string"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 85 --
unicode(6) "string"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 86 --
unicode(6) "string"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 87 --
unicode(6) "string"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 88 --
unicode(6) "string"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 89 --
unicode(6) "string"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 90 --
unicode(6) "string"
bool(true)
NULL
unicode(4) "NULL"
-- Iteration 91 --
unicode(6) "string"
bool(true)
NULL
unicode(4) "NULL"
Done
