--TEST--
Test strncmp() function : usage variations - different lengths(all types)
--FILE--
<?php
/* Prototype  : int strncmp ( string $str1, string $str2, int $len );
 * Description: Binary safe case-sensitive string comparison of the first n characters
 * Source code: Zend/zend_builtin_functions.c
*/

/* Test strncmp() function with the length as all types, and giving the same strings for 'str1' and 'str2' */

echo "*** Test strncmp() function: by supplying all types for 'len' ***\n";

/* definition of required variables */
$str1 = "Hello, World\n";
$str2 = "Hello, World\n";

/* get an unset variable */
$unset_var = 'string_val';
unset($unset_var);

/* get resource handle */
$file_handle = fopen(__FILE__, "r");

/* declaring a class */
class sample  {
  public function __toString() {
  return "object";
  }
}


/* array with different values */
$lengths =  array (
  /* integer values */
  0,
  1,
  12345,

  /* float values */
  10.5,
  10.5e10,
  10.6E-10,
  .5,

  /* hexadecimal values */
  0x12,

  /* octal values */
  012,
  01.2,

  /* array values */
  array(),
  array(0),
  array(1),
  array(1, 2),
  array('color' => 'red', 'item' => 'pen'),

  /* boolean values */
  true,
  false,
  TRUE,
  FALSE,

  /* nulls */
  NULL,
  null,

  /* empty string */
  "",
  '',

  /* undefined variable */
  $undefined_var,

  /* unset variable */
  $unset_var,

  /* resource */
  $file_handle,

  /* object */
  new sample()
);

/* loop through each element of the array and check the working of strncmp() */
$counter = 1;
for($index = 0; $index < count($lengths); $index ++) {
  $len = $lengths[$index];
  echo "-- Iteration $counter --\n";
  var_dump( strncmp($str1, $str2, $len) );
  $counter ++;
}
fclose($file_handle);

echo "*** Done ***\n";
?>
--EXPECTF--
*** Test strncmp() function: by supplying all types for 'len' ***

Notice: Undefined variable: undefined_var in %s on line %d

Notice: Undefined variable: unset_var in %s on line %d
-- Iteration 1 --
int(0)
-- Iteration 2 --
int(0)
-- Iteration 3 --
int(0)
-- Iteration 4 --
int(0)
-- Iteration 5 --
int(0)
-- Iteration 6 --
int(0)
-- Iteration 7 --
int(0)
-- Iteration 8 --
int(0)
-- Iteration 9 --
int(0)
-- Iteration 10 --
int(0)
-- Iteration 11 --

Warning: strncmp() expects parameter 3 to be long, array given in%s on line %d
NULL
-- Iteration 12 --

Warning: strncmp() expects parameter 3 to be long, array given in%s on line %d
NULL
-- Iteration 13 --

Warning: strncmp() expects parameter 3 to be long, array given in%s on line %d
NULL
-- Iteration 14 --

Warning: strncmp() expects parameter 3 to be long, array given in%s on line %d
NULL
-- Iteration 15 --

Warning: strncmp() expects parameter 3 to be long, array given in%s on line %d
NULL
-- Iteration 16 --
int(0)
-- Iteration 17 --
int(0)
-- Iteration 18 --
int(0)
-- Iteration 19 --
int(0)
-- Iteration 20 --
int(0)
-- Iteration 21 --
int(0)
-- Iteration 22 --

Warning: strncmp() expects parameter 3 to be long, string given in%s on line %d
NULL
-- Iteration 23 --

Warning: strncmp() expects parameter 3 to be long, string given in%s on line %d
NULL
-- Iteration 24 --
int(0)
-- Iteration 25 --
int(0)
-- Iteration 26 --

Warning: strncmp() expects parameter 3 to be long, resource given in%s on line %d
NULL
-- Iteration 27 --

Warning: strncmp() expects parameter 3 to be long, object given in%s on line %d
NULL
*** Done ***
