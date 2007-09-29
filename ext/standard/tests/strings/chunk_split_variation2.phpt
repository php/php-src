--TEST--
Test chunk_split() function : usage variations - unexpected values for 'chunklen' argument(Bug#42796)
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
  10.5e10,
  10.6E-10,
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

echo "Done";

//closing resource
fclose($fp);

?>
--EXPECTF--
*** Testing chunk_split() : with unexpected values for 'chunklen' argument ***
-- Iteration 1 --
string(28) "This is ch*uklen vari*ation*"
-- Iteration 2 --

Warning: chunk_split(): Chunk length should be greater than zero in %s on line %d
bool(false)
-- Iteration 3 --
string(26) "This is chuklen variation*"
-- Iteration 4 --

Warning: chunk_split(): Chunk length should be greater than zero in %s on line %d
bool(false)
-- Iteration 5 --

Warning: chunk_split(): Chunk length should be greater than zero in %s on line %d
bool(false)
-- Iteration 6 --

Warning: chunk_split(): Chunk length should be greater than zero in %s on line %d
bool(false)
-- Iteration 7 --
string(50) "T*h*i*s* *i*s* *c*h*u*k*l*e*n* *v*a*r*i*a*t*i*o*n*"
-- Iteration 8 --
string(50) "T*h*i*s* *i*s* *c*h*u*k*l*e*n* *v*a*r*i*a*t*i*o*n*"
-- Iteration 9 --
string(50) "T*h*i*s* *i*s* *c*h*u*k*l*e*n* *v*a*r*i*a*t*i*o*n*"
-- Iteration 10 --
string(50) "T*h*i*s* *i*s* *c*h*u*k*l*e*n* *v*a*r*i*a*t*i*o*n*"
-- Iteration 11 --

Warning: chunk_split(): Chunk length should be greater than zero in %s on line %d
bool(false)
-- Iteration 12 --

Warning: chunk_split(): Chunk length should be greater than zero in %s on line %d
bool(false)
-- Iteration 13 --
string(50) "T*h*i*s* *i*s* *c*h*u*k*l*e*n* *v*a*r*i*a*t*i*o*n*"
-- Iteration 14 --

Warning: chunk_split(): Chunk length should be greater than zero in %s on line %d
bool(false)
-- Iteration 15 --
string(50) "T*h*i*s* *i*s* *c*h*u*k*l*e*n* *v*a*r*i*a*t*i*o*n*"
-- Iteration 16 --

Warning: chunk_split(): Chunk length should be greater than zero in %s on line %d
bool(false)
-- Iteration 17 --

Warning: chunk_split(): Chunk length should be greater than zero in %s on line %d
bool(false)
-- Iteration 18 --

Warning: chunk_split(): Chunk length should be greater than zero in %s on line %d
bool(false)
-- Iteration 19 --

Warning: chunk_split(): Chunk length should be greater than zero in %s on line %d
bool(false)
-- Iteration 20 --

Warning: chunk_split(): Chunk length should be greater than zero in %s on line %d
bool(false)
-- Iteration 21 --

Notice: Object of class MyClass could not be converted to int in %s on line %d
string(50) "T*h*i*s* *i*s* *c*h*u*k*l*e*n* *v*a*r*i*a*t*i*o*n*"
-- Iteration 22 --

Warning: chunk_split(): Chunk length should be greater than zero in %s on line %d
bool(false)
-- Iteration 23 --

Warning: chunk_split(): Chunk length should be greater than zero in %s on line %d
bool(false)
-- Iteration 24 --
string(30) "This *is ch*uklen* vari*ation*"
Done
