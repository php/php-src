--TEST--
Test posix_strerror() function : usage variations
--SKIPIF--
<?php 
	if(!extension_loaded("posix")) print "skip - POSIX extension not loaded"; 
?>
--FILE--
<?php
/* Prototype  : proto string posix_strerror(int errno)
 * Description: Retrieve the system error message associated with the given errno. 
 * Source code: ext/posix/posix.c
 * Alias to functions: 
 */

echo "*** Testing posix_strerror() : usage variations ***\n";

// Initialise function arguments not being substituted (if any)

//get an unset variable
$unset_var = 10;
unset ($unset_var);

//array of values to iterate over
$values = array(

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

      // undefined data
      $undefined_var,

      // unset data
      $unset_var,
      
      // object data
      new stdclass(),
);

// loop through each element of the array for errno

foreach($values as $value) {
      echo "\nArg value $value \n";
      echo gettype( posix_strerror($value) )."\n";
};

echo "Done";
?>
--EXPECTF--
*** Testing posix_strerror() : usage variations ***

Notice: Undefined variable: undefined_var in %s on line %d

Notice: Undefined variable: unset_var in %s on line %d

Arg value 10.5 
string

Arg value -10.5 
string

Arg value 101234567000 
string

Arg value 1.07654321E-9 
string

Arg value 0.5 
string

Notice: Array to string conversion in %sposix_strerror_variation1.php on line %d

Arg value Array 

Warning: posix_strerror() expects parameter 1 to be long, array given in %s on line %d
boolean

Notice: Array to string conversion in %sposix_strerror_variation1.php on line %d

Arg value Array 

Warning: posix_strerror() expects parameter 1 to be long, array given in %s on line %d
boolean

Notice: Array to string conversion in %sposix_strerror_variation1.php on line %d

Arg value Array 

Warning: posix_strerror() expects parameter 1 to be long, array given in %s on line %d
boolean

Notice: Array to string conversion in %sposix_strerror_variation1.php on line %d

Arg value Array 

Warning: posix_strerror() expects parameter 1 to be long, array given in %s on line %d
boolean

Notice: Array to string conversion in %sposix_strerror_variation1.php on line %d

Arg value Array 

Warning: posix_strerror() expects parameter 1 to be long, array given in %s on line %d
boolean

Arg value  
string

Arg value  
string

Arg value 1 
string

Arg value  
string

Arg value 1 
string

Arg value  
string

Arg value  

Warning: posix_strerror() expects parameter 1 to be long, string given in %s on line %d
boolean

Arg value  

Warning: posix_strerror() expects parameter 1 to be long, string given in %s on line %d
boolean

Arg value string 

Warning: posix_strerror() expects parameter 1 to be long, string given in %s on line %d
boolean

Arg value string 

Warning: posix_strerror() expects parameter 1 to be long, string given in %s on line %d
boolean

Arg value  
string

Arg value  
string

Catchable fatal error: Object of class stdClass could not be converted to string in %s on line %d
