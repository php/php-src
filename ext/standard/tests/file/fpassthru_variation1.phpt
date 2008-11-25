--TEST--
Test fpassthru() function : usage variations  - different parameter types
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--FILE--
<?php
/* Prototype  : proto int fpassthru(resource fp)
 * Description: Output all remaining data from a file pointer 
 * Source code: ext/standard/file.c
 * Alias to functions: gzpassthru
 */

/*
 * add a comment here to say what the test is supposed to do
 */

echo "*** Testing fpassthru() : usage variations ***\n";
error_reporting(E_ALL & ~E_NOTICE);

//get an unset variable
$unset_var = 10;
unset ($unset_var);

class testClass {
   public function __toString() {
      return "testClass";
   }
}

//array of values to iterate over
$values = array(

      // int data
      0,
      1,
      12345,
      -2345,

      // float data
      10.5,
      -10.5,
      10.1234567e10,
      10.7654321E-10,
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
      new testClass(),

      // undefined data
      $undefined_var,

      // unset data
      $unset_var,
);

// loop through each element of the array for fp

foreach($values as $value) {
      echo "\nArg value $value \n";
      var_dump( fpassthru($value) );
};

echo "Done";
?>
--EXPECTF--
*** Testing fpassthru() : usage variations ***

Arg value 0 

Warning: fpassthru(): supplied argument is not a valid stream resource in %s on line %d
bool(false)

Arg value 1 

Warning: fpassthru(): supplied argument is not a valid stream resource in %s on line %d
bool(false)

Arg value 12345 

Warning: fpassthru(): supplied argument is not a valid stream resource in %s on line %d
bool(false)

Arg value -2345 

Warning: fpassthru(): supplied argument is not a valid stream resource in %s on line %d
bool(false)

Arg value 10.5 

Warning: fpassthru(): supplied argument is not a valid stream resource in %s on line %d
bool(false)

Arg value -10.5 

Warning: fpassthru(): supplied argument is not a valid stream resource in %s on line %d
bool(false)

Arg value 101234567000 

Warning: fpassthru(): supplied argument is not a valid stream resource in %s on line %d
bool(false)

Arg value 1.07654321E-9 

Warning: fpassthru(): supplied argument is not a valid stream resource in %s on line %d
bool(false)

Arg value 0.5 

Warning: fpassthru(): supplied argument is not a valid stream resource in %s on line %d
bool(false)

Arg value Array 

Warning: fpassthru(): supplied argument is not a valid stream resource in %s on line %d
bool(false)

Arg value Array 

Warning: fpassthru(): supplied argument is not a valid stream resource in %s on line %d
bool(false)

Arg value Array 

Warning: fpassthru(): supplied argument is not a valid stream resource in %s on line %d
bool(false)

Arg value Array 

Warning: fpassthru(): supplied argument is not a valid stream resource in %s on line %d
bool(false)

Arg value Array 

Warning: fpassthru(): supplied argument is not a valid stream resource in %s on line %d
bool(false)

Arg value  

Warning: fpassthru(): supplied argument is not a valid stream resource in %s on line %d
bool(false)

Arg value  

Warning: fpassthru(): supplied argument is not a valid stream resource in %s on line %d
bool(false)

Arg value 1 

Warning: fpassthru(): supplied argument is not a valid stream resource in %s on line %d
bool(false)

Arg value  

Warning: fpassthru(): supplied argument is not a valid stream resource in %s on line %d
bool(false)

Arg value 1 

Warning: fpassthru(): supplied argument is not a valid stream resource in %s on line %d
bool(false)

Arg value  

Warning: fpassthru(): supplied argument is not a valid stream resource in %s on line %d
bool(false)

Arg value  

Warning: fpassthru(): supplied argument is not a valid stream resource in %s on line %d
bool(false)

Arg value  

Warning: fpassthru(): supplied argument is not a valid stream resource in %s on line %d
bool(false)

Arg value string 

Warning: fpassthru(): supplied argument is not a valid stream resource in %s on line %d
bool(false)

Arg value string 

Warning: fpassthru(): supplied argument is not a valid stream resource in %s on line %d
bool(false)

Arg value testClass 

Warning: fpassthru(): supplied argument is not a valid stream resource in %s on line %d
bool(false)

Arg value  

Warning: fpassthru(): supplied argument is not a valid stream resource in %s on line %d
bool(false)

Arg value  

Warning: fpassthru(): supplied argument is not a valid stream resource in %s on line %d
bool(false)
Done
