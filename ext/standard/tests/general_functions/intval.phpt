--TEST--
Test intval() function
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only");
?>
--FILE--
<?php
/* Prototype: int intval( mixed $var [.int $base] );
 * Description: Returns the integer value of var, using the specified base for the conversion(the default is base 10).
 */

echo "*** Testing intval() with valid integer values ***\n";
// different valid  integer values
$valid_ints = array(
                '0',
                '1',
                '-1',
                '-2147483648', // max negative integer value
                '-2147483647',
                2147483647,  // max positive integer value
                2147483640,
                0x123B,      // integer as hexadecimal
                '0x12ab',
                '0Xfff',
                '0XFA',
                -0x80000000, // max negative integer as hexadecimal
                '0x7fffffff',  // max positive integer as hexadecimal
                0x7FFFFFFF,  // max positive integer as hexadecimal
                '0123',        // integer as octal
                01,       // should be quivalent to octal 1
                -020000000000, // max negative integer as octal
                017777777777,  // max positive integer as octal
               );

/* loop to check that intval() recognizes different
   integer values, expected output:integer value in decimal notation for valid integer */

echo "\n***Output with default base value ie 10 ***\n";
foreach ($valid_ints as $value ) {
   var_dump( intval($value) );
}


echo "\n***Output with base value of 10( explicitly passed as argument) ***\n";
foreach ($valid_ints as $value ) {
   var_dump( intval($value, 10) );
}


echo "\n***Output with base value  of 16 ***\n";
foreach ($valid_ints as $value ) {
   var_dump( intval($value, 16) );
}

echo "\n***Output with base value of 8 ***\n";
foreach ($valid_ints as $value ) {
   var_dump( intval($value, 8) );
}

echo "\n*** Testing intval() on non integer types ***\n";

// get a resource type variable
$fp = fopen (__FILE__, "r");
fclose($fp);
$dfp = opendir ( __DIR__ );
closedir($dfp);

// unset variable

$unset_var = 10;
unset ($unset_var);

// other types in a array
$not_int_types = array (
  /* float values */
  '-2147483649', // float value
  '2147483648',  // float value
  '-0x80000001', // float value, beyond max negative int
  '0x800000001', // float value, beyond max positive int
  '020000000001', // float value, beyond max positive int
  '-020000000001', // float value, beyond max negative int
  0.0,
  -0.1,
  1.0,
  1e5,
  -1e6,
  1E8,
  -1E9,
  10.0000000000000000005,
  10.5e+5,

  /* resources */
  $fp,
  $dfp,

  /* arrays */
  array(),
  array(0),
  array(1),
  array(NULL),
  array(null),
  array("string"),
  array(true),
  array(TRUE),
  array(false),
  array(FALSE),
  array(1,2,3,4),
  array(1 => "One", "two" => 2),

  /* strings */
  "",
  '',
  "0",
  '0',
  "1",
  '1',
  "\x01",
  '\x01',
  "\01",
  '\01',
  'string',
  "string",
  "true",
  "FALSE",
  'false',
  'TRUE',
  "NULL",
  'null',

  /* booleans */
  true,
  false,
  TRUE,
  FALSE,

  /* undefined and unset vars */
  @$unset_var,
  @$undefined_var
);


/* loop through the $not_int_types to see working of
   intval() on non integer types, expected output: integer value in decimal notation for valid integers */
foreach ($not_int_types as $type ) {
   var_dump( intval($type) );
}

echo "\n*** Testing error conditions ***\n";
//Zero argument
var_dump( intval() );

//arguments more than expected
var_dump( intval(TRUE, FALSE, TRUE) );

echo "\n--- Done ---\n";


?>
--EXPECTF--
*** Testing intval() with valid integer values ***

***Output with default base value ie 10 ***
int(0)
int(1)
int(-1)
int(-2147483648)
int(-2147483647)
int(2147483647)
int(2147483640)
int(4667)
int(0)
int(0)
int(0)
int(-2147483648)
int(0)
int(2147483647)
int(123)
int(1)
int(-2147483648)
int(2147483647)

***Output with base value of 10( explicitly passed as argument) ***
int(0)
int(1)
int(-1)
int(-2147483648)
int(-2147483647)
int(2147483647)
int(2147483640)
int(4667)
int(0)
int(0)
int(0)
int(-2147483648)
int(0)
int(2147483647)
int(123)
int(1)
int(-2147483648)
int(2147483647)

***Output with base value  of 16 ***
int(0)
int(1)
int(-1)
int(-2147483648)
int(-2147483648)
int(2147483647)
int(2147483640)
int(4667)
int(4779)
int(4095)
int(250)
int(-2147483648)
int(2147483647)
int(2147483647)
int(291)
int(1)
int(-2147483648)
int(2147483647)

***Output with base value of 8 ***
int(0)
int(1)
int(-1)
int(-9020)
int(-9020)
int(2147483647)
int(2147483640)
int(4667)
int(0)
int(0)
int(0)
int(-2147483648)
int(0)
int(2147483647)
int(83)
int(1)
int(-2147483648)
int(2147483647)

*** Testing intval() on non integer types ***
int(-2147483648)
int(2147483647)
int(0)
int(0)
int(2147483647)
int(-2147483648)
int(0)
int(0)
int(1)
int(100000)
int(-1000000)
int(100000000)
int(-1000000000)
int(10)
int(1050000)
int(%d)
int(%d)
int(0)
int(1)
int(1)
int(1)
int(1)
int(1)
int(1)
int(1)
int(1)
int(1)
int(1)
int(1)
int(0)
int(0)
int(0)
int(0)
int(1)
int(1)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(1)
int(0)
int(1)
int(0)
int(0)
int(0)

*** Testing error conditions ***

Warning: intval() expects at least 1 parameter, 0 given in %s on line %d
NULL

Warning: intval() expects at most 2 parameters, 3 given in %s on line %d
NULL

--- Done ---
