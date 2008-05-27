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
         settype() to bool/boolean type.
   Set type of the data to "bool"/"boolean" and verify using gettype
   Following are performed in the listed sequence:
     get the current type of the variable
     set the type of the variable to bool/boolean type
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

class class_with_no_member {
 // no member(s)
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
  "0",
  '0',

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
  0,
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
  0.0,
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
  new class_with_no_member,

  /* undefined/unset vars */
  $unset_var,
  $undef_var,
  
  /* binary strings */
  b"0",
  b'0',
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

// test conversion to these types
$types = array(
  "boolean",
  "bool"
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
--EXPECT--
8: Undefined variable: unset_var
8: Undefined variable: undef_var

*** Testing settype() & gettype() : usage variations ***

-- Setting type of data to boolean --
-- Iteration 1 --
unicode(4) "NULL"
bool(true)
bool(false)
unicode(7) "boolean"
-- Iteration 2 --
unicode(7) "boolean"
bool(true)
bool(false)
unicode(7) "boolean"
-- Iteration 3 --
unicode(7) "boolean"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 4 --
unicode(7) "boolean"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 5 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 6 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 7 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 8 --
unicode(7) "unicode"
bool(true)
bool(false)
unicode(7) "boolean"
-- Iteration 9 --
unicode(7) "unicode"
bool(true)
bool(false)
unicode(7) "boolean"
-- Iteration 10 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 11 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 12 --
unicode(7) "unicode"
bool(true)
bool(false)
unicode(7) "boolean"
-- Iteration 13 --
unicode(7) "unicode"
bool(true)
bool(false)
unicode(7) "boolean"
-- Iteration 14 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 15 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 16 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 17 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 18 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 19 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 20 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 21 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 22 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 23 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 24 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 25 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 26 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 27 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 28 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 29 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 30 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 31 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 32 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 33 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 34 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 35 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 36 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 37 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 38 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 39 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 40 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 41 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 42 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 43 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 44 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 45 --
unicode(5) "array"
bool(true)
bool(false)
unicode(7) "boolean"
-- Iteration 46 --
unicode(5) "array"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 47 --
unicode(5) "array"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 48 --
unicode(5) "array"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 49 --
unicode(5) "array"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 50 --
unicode(7) "integer"
bool(true)
bool(false)
unicode(7) "boolean"
-- Iteration 51 --
unicode(6) "double"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 52 --
unicode(7) "integer"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 53 --
unicode(6) "double"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 54 --
unicode(6) "double"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 55 --
unicode(7) "integer"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 56 --
unicode(6) "double"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 57 --
unicode(7) "integer"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 58 --
unicode(7) "integer"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 59 --
unicode(7) "integer"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 60 --
unicode(6) "double"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 61 --
unicode(6) "double"
bool(true)
bool(false)
unicode(7) "boolean"
-- Iteration 62 --
unicode(6) "double"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 63 --
unicode(6) "double"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 64 --
unicode(6) "double"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 65 --
unicode(6) "double"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 66 --
unicode(6) "double"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 67 --
unicode(6) "double"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 68 --
unicode(6) "double"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 69 --
unicode(6) "double"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 70 --
unicode(6) "double"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 71 --
unicode(6) "double"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 72 --
unicode(6) "double"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 73 --
unicode(6) "double"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 74 --
unicode(6) "double"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 75 --
unicode(6) "double"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 76 --
unicode(6) "double"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 77 --
unicode(6) "double"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 78 --
unicode(6) "double"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 79 --
unicode(6) "double"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 80 --
unicode(6) "object"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 81 --
unicode(6) "object"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 82 --
unicode(6) "object"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 83 --
unicode(6) "object"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 84 --
unicode(4) "NULL"
bool(true)
bool(false)
unicode(7) "boolean"
-- Iteration 85 --
unicode(4) "NULL"
bool(true)
bool(false)
unicode(7) "boolean"
-- Iteration 86 --
unicode(6) "string"
bool(true)
bool(false)
unicode(7) "boolean"
-- Iteration 87 --
unicode(6) "string"
bool(true)
bool(false)
unicode(7) "boolean"
-- Iteration 88 --
unicode(6) "string"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 89 --
unicode(6) "string"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 90 --
unicode(6) "string"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 91 --
unicode(6) "string"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 92 --
unicode(6) "string"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 93 --
unicode(6) "string"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 94 --
unicode(6) "string"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 95 --
unicode(6) "string"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 96 --
unicode(6) "string"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 97 --
unicode(6) "string"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 98 --
unicode(6) "string"
bool(true)
bool(true)
unicode(7) "boolean"

-- Setting type of data to bool --
-- Iteration 1 --
unicode(4) "NULL"
bool(true)
bool(false)
unicode(7) "boolean"
-- Iteration 2 --
unicode(7) "boolean"
bool(true)
bool(false)
unicode(7) "boolean"
-- Iteration 3 --
unicode(7) "boolean"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 4 --
unicode(7) "boolean"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 5 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 6 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 7 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 8 --
unicode(7) "unicode"
bool(true)
bool(false)
unicode(7) "boolean"
-- Iteration 9 --
unicode(7) "unicode"
bool(true)
bool(false)
unicode(7) "boolean"
-- Iteration 10 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 11 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 12 --
unicode(7) "unicode"
bool(true)
bool(false)
unicode(7) "boolean"
-- Iteration 13 --
unicode(7) "unicode"
bool(true)
bool(false)
unicode(7) "boolean"
-- Iteration 14 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 15 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 16 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 17 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 18 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 19 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 20 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 21 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 22 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 23 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 24 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 25 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 26 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 27 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 28 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 29 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 30 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 31 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 32 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 33 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 34 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 35 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 36 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 37 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 38 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 39 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 40 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 41 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 42 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 43 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 44 --
unicode(7) "unicode"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 45 --
unicode(5) "array"
bool(true)
bool(false)
unicode(7) "boolean"
-- Iteration 46 --
unicode(5) "array"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 47 --
unicode(5) "array"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 48 --
unicode(5) "array"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 49 --
unicode(5) "array"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 50 --
unicode(7) "integer"
bool(true)
bool(false)
unicode(7) "boolean"
-- Iteration 51 --
unicode(6) "double"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 52 --
unicode(7) "integer"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 53 --
unicode(6) "double"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 54 --
unicode(6) "double"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 55 --
unicode(7) "integer"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 56 --
unicode(6) "double"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 57 --
unicode(7) "integer"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 58 --
unicode(7) "integer"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 59 --
unicode(7) "integer"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 60 --
unicode(6) "double"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 61 --
unicode(6) "double"
bool(true)
bool(false)
unicode(7) "boolean"
-- Iteration 62 --
unicode(6) "double"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 63 --
unicode(6) "double"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 64 --
unicode(6) "double"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 65 --
unicode(6) "double"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 66 --
unicode(6) "double"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 67 --
unicode(6) "double"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 68 --
unicode(6) "double"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 69 --
unicode(6) "double"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 70 --
unicode(6) "double"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 71 --
unicode(6) "double"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 72 --
unicode(6) "double"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 73 --
unicode(6) "double"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 74 --
unicode(6) "double"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 75 --
unicode(6) "double"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 76 --
unicode(6) "double"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 77 --
unicode(6) "double"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 78 --
unicode(6) "double"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 79 --
unicode(6) "double"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 80 --
unicode(6) "object"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 81 --
unicode(6) "object"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 82 --
unicode(6) "object"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 83 --
unicode(6) "object"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 84 --
unicode(4) "NULL"
bool(true)
bool(false)
unicode(7) "boolean"
-- Iteration 85 --
unicode(4) "NULL"
bool(true)
bool(false)
unicode(7) "boolean"
-- Iteration 86 --
unicode(6) "string"
bool(true)
bool(false)
unicode(7) "boolean"
-- Iteration 87 --
unicode(6) "string"
bool(true)
bool(false)
unicode(7) "boolean"
-- Iteration 88 --
unicode(6) "string"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 89 --
unicode(6) "string"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 90 --
unicode(6) "string"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 91 --
unicode(6) "string"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 92 --
unicode(6) "string"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 93 --
unicode(6) "string"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 94 --
unicode(6) "string"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 95 --
unicode(6) "string"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 96 --
unicode(6) "string"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 97 --
unicode(6) "string"
bool(true)
bool(true)
unicode(7) "boolean"
-- Iteration 98 --
unicode(6) "string"
bool(true)
bool(true)
unicode(7) "boolean"
Done
