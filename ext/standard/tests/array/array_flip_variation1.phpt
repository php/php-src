--TEST--
Test array_flip() function : usage variations - unexpected values for 'input' argument
--FILE--
<?php
/* Prototype  : array array_flip(array $input)
 * Description: Return array with key <-> value flipped 
 * Source code: ext/standard/array.c
*/

echo "*** Testing array_flip() : usage variations - unexpected values for 'input' ***\n";

//get an unset variable
$unset_var = 10;
unset ($unset_var);

//class definition for object variable
class MyClass
{
  public function __toString()
  {
     return 'object';
  }
}

//resource variable
$fp = fopen(__FILE__,'r');
    
//array of values for 'input' argument
$values = array(
          // int data
  /*1*/   0,
          1,
          12345,
          -2345,

          // float data
  /*5*/   10.5,
          -10.5,
          10.5e10,
          10.6E-10,
          .5,

          // null data
  /*10*/  NULL,
          null,

          // boolean data
  /*12*/  true,
          false,
          TRUE,
  /*15*/  FALSE,

          // empty data
          "",
          '',

          // string data
          "string",
          'string',

          // object data
  /*20*/  new MyClass(),

          // undefined data
          @$undefined_var,

          // unset data
          @$unset_var,

          //resource data
  /*23*/  $fp
);

// loop through each element of $values for 'input' argument
for($count = 0; $count < count($values); $count++) {
  echo "-- Iteration ".($count + 1). " --\n";
  var_dump( array_flip($values[$count]) );
};

//closing resource
fclose($fp);

echo "Done"
?>
--EXPECTF--
*** Testing array_flip() : usage variations - unexpected values for 'input' ***
-- Iteration 1 --

Warning: array_flip(): The argument should be an array in %s on line %d
bool(false)
-- Iteration 2 --

Warning: array_flip(): The argument should be an array in %s on line %d
bool(false)
-- Iteration 3 --

Warning: array_flip(): The argument should be an array in %s on line %d
bool(false)
-- Iteration 4 --

Warning: array_flip(): The argument should be an array in %s on line %d
bool(false)
-- Iteration 5 --

Warning: array_flip(): The argument should be an array in %s on line %d
bool(false)
-- Iteration 6 --

Warning: array_flip(): The argument should be an array in %s on line %d
bool(false)
-- Iteration 7 --

Warning: array_flip(): The argument should be an array in %s on line %d
bool(false)
-- Iteration 8 --

Warning: array_flip(): The argument should be an array in %s on line %d
bool(false)
-- Iteration 9 --

Warning: array_flip(): The argument should be an array in %s on line %d
bool(false)
-- Iteration 10 --

Warning: array_flip(): The argument should be an array in %s on line %d
bool(false)
-- Iteration 11 --

Warning: array_flip(): The argument should be an array in %s on line %d
bool(false)
-- Iteration 12 --

Warning: array_flip(): The argument should be an array in %s on line %d
bool(false)
-- Iteration 13 --

Warning: array_flip(): The argument should be an array in %s on line %d
bool(false)
-- Iteration 14 --

Warning: array_flip(): The argument should be an array in %s on line %d
bool(false)
-- Iteration 15 --

Warning: array_flip(): The argument should be an array in %s on line %d
bool(false)
-- Iteration 16 --

Warning: array_flip(): The argument should be an array in %s on line %d
bool(false)
-- Iteration 17 --

Warning: array_flip(): The argument should be an array in %s on line %d
bool(false)
-- Iteration 18 --

Warning: array_flip(): The argument should be an array in %s on line %d
bool(false)
-- Iteration 19 --

Warning: array_flip(): The argument should be an array in %s on line %d
bool(false)
-- Iteration 20 --
array(0) {
}
-- Iteration 21 --

Warning: array_flip(): The argument should be an array in %s on line %d
bool(false)
-- Iteration 22 --

Warning: array_flip(): The argument should be an array in %s on line %d
bool(false)
-- Iteration 23 --

Warning: array_flip(): The argument should be an array in %s on line %d
bool(false)
Done
