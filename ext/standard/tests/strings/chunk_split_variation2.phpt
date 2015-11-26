--TEST--
Test chunk_split() function : usage variations - unexpected values for 'chunklen' argument(Bug#42796)
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64-bit only");
--FILE--
<?php
/* Prototype  : string chunk_split(string $str [, int $chunklen [, string $ending]])
 * Description: Returns split line
 * Source code: ext/standard/string.c
 * Alias to functions: none
*/

echo "*** Testing chunk_split() : with unexpected values for 'chunklen' argument ***\n";

// Initialise function arguments
$str = 'This is chuklen variation';
$ending = '*';

//get an unset variable
$unset_var = 10;
unset ($unset_var);

//get resource variable
$fp = fopen(__FILE__, 'r');

//Class to get object variable
class MyClass
{
   public function __toString() {
     return "object";
   }
}

//array of values to iterate over
$values = array(

  // float data
  10.5,
  -10.5,
  (float) PHP_INT_MAX + 1,
  (float) -PHP_INT_MAX - 1,
  .5,
 
  // array data
  array(),
  array(0),
  array(1),
  array(1, 2),
  array('color' => 'red', 'item' => 'pen'),

  // null data
  NULL,
  null,

  // boolean data
  true,
  false,
  TRUE,
  FALSE,

  // empty data
  "",
  '',

  // string data
  "string",
  'string',

  // object data
  new MyClass(),

  // undefined data
  @$undefined_var,

  // unset data
  @$unset_var,

  // resource variable
  $fp
);

// loop through each element of the values for 'chunklen'
for($count = 0; $count < count($values); $count++) {
  echo "-- Iteration ".($count+1)." --\n";
  var_dump( chunk_split($str, $values[$count], $ending) );
}

//closing resource
fclose($fp);

?>
===DONE===
--EXPECTF--
*** Testing chunk_split() : with unexpected values for 'chunklen' argument ***
-- Iteration 1 --
string(28) "This is ch*uklen vari*ation*"
-- Iteration 2 --

Warning: chunk_split(): Chunk length should be greater than zero in %schunk_split_variation2.php on line %d
bool(false)
-- Iteration 3 --

Warning: chunk_split() expects parameter 2 to be integer, float given in %s on line %d
NULL
-- Iteration 4 --

Warning: chunk_split(): Chunk length should be greater than zero in %schunk_split_variation2.php on line %d
bool(false)
-- Iteration 5 --

Warning: chunk_split(): Chunk length should be greater than zero in %schunk_split_variation2.php on line %d
bool(false)
-- Iteration 6 --

Warning: chunk_split() expects parameter 2 to be integer, array given in %schunk_split_variation2.php on line %d
NULL
-- Iteration 7 --

Warning: chunk_split() expects parameter 2 to be integer, array given in %schunk_split_variation2.php on line %d
NULL
-- Iteration 8 --

Warning: chunk_split() expects parameter 2 to be integer, array given in %schunk_split_variation2.php on line %d
NULL
-- Iteration 9 --

Warning: chunk_split() expects parameter 2 to be integer, array given in %schunk_split_variation2.php on line %d
NULL
-- Iteration 10 --

Warning: chunk_split() expects parameter 2 to be integer, array given in %schunk_split_variation2.php on line %d
NULL
-- Iteration 11 --

Warning: chunk_split(): Chunk length should be greater than zero in %schunk_split_variation2.php on line %d
bool(false)
-- Iteration 12 --

Warning: chunk_split(): Chunk length should be greater than zero in %schunk_split_variation2.php on line %d
bool(false)
-- Iteration 13 --
string(50) "T*h*i*s* *i*s* *c*h*u*k*l*e*n* *v*a*r*i*a*t*i*o*n*"
-- Iteration 14 --

Warning: chunk_split(): Chunk length should be greater than zero in %schunk_split_variation2.php on line %d
bool(false)
-- Iteration 15 --
string(50) "T*h*i*s* *i*s* *c*h*u*k*l*e*n* *v*a*r*i*a*t*i*o*n*"
-- Iteration 16 --

Warning: chunk_split(): Chunk length should be greater than zero in %schunk_split_variation2.php on line %d
bool(false)
-- Iteration 17 --

Warning: chunk_split() expects parameter 2 to be integer, string given in %schunk_split_variation2.php on line %d
NULL
-- Iteration 18 --

Warning: chunk_split() expects parameter 2 to be integer, string given in %schunk_split_variation2.php on line %d
NULL
-- Iteration 19 --

Warning: chunk_split() expects parameter 2 to be integer, string given in %schunk_split_variation2.php on line %d
NULL
-- Iteration 20 --

Warning: chunk_split() expects parameter 2 to be integer, string given in %schunk_split_variation2.php on line %d
NULL
-- Iteration 21 --

Warning: chunk_split() expects parameter 2 to be integer, object given in %schunk_split_variation2.php on line %d
NULL
-- Iteration 22 --

Warning: chunk_split(): Chunk length should be greater than zero in %schunk_split_variation2.php on line %d
bool(false)
-- Iteration 23 --

Warning: chunk_split(): Chunk length should be greater than zero in %schunk_split_variation2.php on line %d
bool(false)
-- Iteration 24 --

Warning: chunk_split() expects parameter 2 to be integer, resource given in %schunk_split_variation2.php on line %d
NULL
===DONE===
