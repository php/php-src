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

  function __construct($x, $y) {
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
--EXPECTF--	
8: Undefined variable: unset_var
8: Undefined variable: undef_var

*** Testing settype() & gettype() : usage variations ***

-- Setting type of data to boolean --
-- Iteration 1 --
string(4) "NULL"
bool(true)
bool(false)
string(7) "boolean"
-- Iteration 2 --
string(7) "boolean"
bool(true)
bool(false)
string(7) "boolean"
-- Iteration 3 --
string(7) "boolean"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 4 --
string(7) "boolean"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 5 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 6 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 7 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 8 --
string(6) "string"
bool(true)
bool(false)
string(7) "boolean"
-- Iteration 9 --
string(6) "string"
bool(true)
bool(false)
string(7) "boolean"
-- Iteration 10 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 11 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 12 --
string(6) "string"
bool(true)
bool(false)
string(7) "boolean"
-- Iteration 13 --
string(6) "string"
bool(true)
bool(false)
string(7) "boolean"
-- Iteration 14 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 15 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 16 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 17 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 18 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 19 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 20 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 21 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 22 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 23 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 24 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 25 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 26 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 27 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 28 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 29 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 30 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 31 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 32 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 33 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 34 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 35 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 36 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 37 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 38 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 39 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 40 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 41 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 42 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 43 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 44 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 45 --
string(5) "array"
bool(true)
bool(false)
string(7) "boolean"
-- Iteration 46 --
string(5) "array"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 47 --
string(5) "array"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 48 --
string(5) "array"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 49 --
string(5) "array"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 50 --
string(7) "integer"
bool(true)
bool(false)
string(7) "boolean"
-- Iteration 51 --
string(6) "double"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 52 --
string(7) "integer"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 53 --
string(6) "double"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 54 --
string(6) "double"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 55 --
string(7) "integer"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 56 --
string(6) "double"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 57 --
string(7) "integer"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 58 --
string(7) "integer"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 59 --
string(7) "integer"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 60 --
string(6) "double"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 61 --
string(6) "double"
bool(true)
bool(false)
string(7) "boolean"
-- Iteration 62 --
string(6) "double"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 63 --
string(6) "double"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 64 --
string(6) "double"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 65 --
string(6) "double"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 66 --
string(6) "double"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 67 --
string(6) "double"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 68 --
string(6) "double"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 69 --
string(6) "double"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 70 --
string(6) "double"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 71 --
string(6) "double"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 72 --
string(6) "double"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 73 --
string(6) "double"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 74 --
string(6) "double"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 75 --
string(6) "double"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 76 --
string(6) "double"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 77 --
string(6) "double"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 78 --
string(6) "double"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 79 --
string(6) "double"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 80 --
string(6) "object"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 81 --
string(6) "object"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 82 --
string(6) "object"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 83 --
string(6) "object"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 84 --
string(4) "NULL"
bool(true)
bool(false)
string(7) "boolean"
-- Iteration 85 --
string(4) "NULL"
bool(true)
bool(false)
string(7) "boolean"
-- Iteration 86 --
string(6) "string"
bool(true)
bool(false)
string(7) "boolean"
-- Iteration 87 --
string(6) "string"
bool(true)
bool(false)
string(7) "boolean"
-- Iteration 88 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 89 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 90 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 91 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 92 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 93 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 94 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 95 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 96 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 97 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 98 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"

-- Setting type of data to bool --
-- Iteration 1 --
string(4) "NULL"
bool(true)
bool(false)
string(7) "boolean"
-- Iteration 2 --
string(7) "boolean"
bool(true)
bool(false)
string(7) "boolean"
-- Iteration 3 --
string(7) "boolean"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 4 --
string(7) "boolean"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 5 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 6 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 7 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 8 --
string(6) "string"
bool(true)
bool(false)
string(7) "boolean"
-- Iteration 9 --
string(6) "string"
bool(true)
bool(false)
string(7) "boolean"
-- Iteration 10 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 11 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 12 --
string(6) "string"
bool(true)
bool(false)
string(7) "boolean"
-- Iteration 13 --
string(6) "string"
bool(true)
bool(false)
string(7) "boolean"
-- Iteration 14 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 15 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 16 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 17 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 18 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 19 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 20 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 21 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 22 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 23 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 24 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 25 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 26 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 27 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 28 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 29 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 30 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 31 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 32 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 33 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 34 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 35 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 36 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 37 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 38 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 39 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 40 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 41 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 42 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 43 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 44 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 45 --
string(5) "array"
bool(true)
bool(false)
string(7) "boolean"
-- Iteration 46 --
string(5) "array"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 47 --
string(5) "array"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 48 --
string(5) "array"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 49 --
string(5) "array"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 50 --
string(7) "integer"
bool(true)
bool(false)
string(7) "boolean"
-- Iteration 51 --
string(6) "double"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 52 --
string(7) "integer"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 53 --
string(6) "double"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 54 --
string(6) "double"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 55 --
string(7) "integer"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 56 --
string(6) "double"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 57 --
string(7) "integer"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 58 --
string(7) "integer"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 59 --
string(7) "integer"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 60 --
string(6) "double"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 61 --
string(6) "double"
bool(true)
bool(false)
string(7) "boolean"
-- Iteration 62 --
string(6) "double"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 63 --
string(6) "double"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 64 --
string(6) "double"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 65 --
string(6) "double"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 66 --
string(6) "double"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 67 --
string(6) "double"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 68 --
string(6) "double"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 69 --
string(6) "double"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 70 --
string(6) "double"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 71 --
string(6) "double"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 72 --
string(6) "double"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 73 --
string(6) "double"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 74 --
string(6) "double"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 75 --
string(6) "double"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 76 --
string(6) "double"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 77 --
string(6) "double"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 78 --
string(6) "double"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 79 --
string(6) "double"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 80 --
string(6) "object"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 81 --
string(6) "object"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 82 --
string(6) "object"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 83 --
string(6) "object"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 84 --
string(4) "NULL"
bool(true)
bool(false)
string(7) "boolean"
-- Iteration 85 --
string(4) "NULL"
bool(true)
bool(false)
string(7) "boolean"
-- Iteration 86 --
string(6) "string"
bool(true)
bool(false)
string(7) "boolean"
-- Iteration 87 --
string(6) "string"
bool(true)
bool(false)
string(7) "boolean"
-- Iteration 88 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 89 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 90 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 91 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 92 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 93 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 94 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 95 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 96 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 97 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
-- Iteration 98 --
string(6) "string"
bool(true)
bool(true)
string(7) "boolean"
Done
