--TEST--
Test strncmp() function : usage variations - different inputs(all types)
--FILE--
<?php
/* Prototype  : int strncmp ( string $str1, string $str2, int $len );
 * Description: Binary safe case-sensitive string comparison of the first n characters
 * Source code: Zend/zend_builtin_functions.c
*/

/* Test strncmp() function with the input strings are of all types */

echo "*** Testing strncmp() function: by supplying all types for 'str1' and 'str2' ***\n";
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
$values =  array (
  /* integer values */
  0,
  1,
  12345,
  -2345,

  /* float values */
  10.5,
  -10.5,
  10.5e10,
  10.6E-10,
  .5,

  /* hexadecimal values */
  0x12,
  -0x12,

  /* octal values */
  012,
  -012,
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
for($index = 0; $index < count($values); $index ++) {
  echo "-- Iteration $counter --\n";
  $str1 = $values[$index];
  $str2 = $values[$index];
  $len = strlen($values[$index]) + 1;
  var_dump( strncmp($str1, $str2, $len) );
  $counter ++;
}
fclose($file_handle);

echo "*** Done ***\n";
?>
--EXPECTF--
*** Testing strncmp() function: by supplying all types for 'str1' and 'str2' ***

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
int(0)
-- Iteration 12 --
int(0)
-- Iteration 13 --
int(0)
-- Iteration 14 --
int(0)
-- Iteration 15 --

Warning: strlen() expects parameter 1 to be string, array given in %s on line %d

Warning: strncmp() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 16 --

Warning: strlen() expects parameter 1 to be string, array given in %s on line %d

Warning: strncmp() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 17 --

Warning: strlen() expects parameter 1 to be string, array given in %s on line %d

Warning: strncmp() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 18 --

Warning: strlen() expects parameter 1 to be string, array given in %s on line %d

Warning: strncmp() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 19 --

Warning: strlen() expects parameter 1 to be string, array given in %s on line %d

Warning: strncmp() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 20 --
int(0)
-- Iteration 21 --
int(0)
-- Iteration 22 --
int(0)
-- Iteration 23 --
int(0)
-- Iteration 24 --
int(0)
-- Iteration 25 --
int(0)
-- Iteration 26 --
int(0)
-- Iteration 27 --
int(0)
-- Iteration 28 --
int(0)
-- Iteration 29 --
int(0)
-- Iteration 30 --

Warning: strlen() expects parameter 1 to be string, resource given in %s on line %d

Warning: strncmp() expects parameter 1 to be string, resource given in %s on line %d
NULL
-- Iteration 31 --
int(0)
*** Done ***
