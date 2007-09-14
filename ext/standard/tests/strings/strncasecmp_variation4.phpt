--TEST--
Test strncasecmp() function : usage variations - unexpected values for 'str1' & 'str2'
--FILE--
<?php
/* Prototype  : int strncasecmp ( string $str1, string $str2, int $len );
 * Description: Binary safe case-insensitive string comparison of the first n characters
 * Source code: Zend/zend_builtin_functions.c
*/

/* Test strncasecmp() function with the unexpected inputs for 'str1' and 'str2' */

echo "*** Testing strncasecmp() function: with unexpected values for 'str1' and 'str2' ***\n";
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
  @$undefined_var,

  /* unset variable */
  @$unset_var,

  /* resource */
  $file_handle,  

  /* object */
  new sample()
);

/* loop through each element of the array and check the working of strncasecmp() */
$counter = 1;
for($index = 0; $index < count($values); $index ++) {
  echo "-- Iteration $counter --\n";
  $str1 = $values[$index];
  $str2 = $values[$index];
  $len = strlen($values[$index]) + 1;
  var_dump( strncasecmp($str1, $str2, $len) );
  $counter ++;
}

fclose($file_handle);  //closing the file handle

echo "*** Done ***\n";
?>
--EXPECTF--
*** Testing strncasecmp() function: with unexpected values for 'str1' and 'str2' ***
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

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d
int(0)
-- Iteration 16 --

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d
int(0)
-- Iteration 17 --

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d
int(0)
-- Iteration 18 --

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d
int(0)
-- Iteration 19 --

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d
int(0)
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
int(0)
-- Iteration 31 --
int(0)
*** Done ***
