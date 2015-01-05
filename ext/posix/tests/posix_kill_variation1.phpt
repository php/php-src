--TEST--
Test posix_kill() function : usage variations  - first parameter type
--SKIPIF--
<?php
    PHP_INT_SIZE == 4 or die("skip - 32-bit only");
	if(!extension_loaded("posix")) print "skip - POSIX extension not loaded"; 
?>
--FILE--
<?php
/* Prototype  : proto bool posix_kill(int pid, int sig)
 * Description: Send a signal to a process (POSIX.1, 3.3.2) 
 * Source code: ext/posix/posix.c
 * Alias to functions: 
 */

echo "*** Testing posix_kill() : usage variations ***\n";

// Initialise function arguments not being substituted (if any)
$sig = -999;

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

// loop through each element of the array for pid

foreach($values as $value) {
      echo "\nArg value $value \n";
      var_dump( posix_kill($value, $sig) );
};

echo "Done";
?>
--EXPECTF--
*** Testing posix_kill() : usage variations ***

Notice: Undefined variable: undefined_var in %s on line %d

Notice: Undefined variable: unset_var in %s on line %d

Arg value 10.5 
bool(false)

Arg value -10.5 
bool(false)

Arg value 101234567000 

Warning: posix_kill() expects parameter 1 to be integer, float given in %s on line %d
bool(false)

Arg value 1.07654321E-9 
bool(false)

Arg value 0.5 
bool(false)

Notice: Array to string conversion in %sposix_kill_variation1.php on line %d

Arg value Array 

Warning: posix_kill() expects parameter 1 to be integer, array given in %s on line %d
bool(false)

Notice: Array to string conversion in %sposix_kill_variation1.php on line %d

Arg value Array 

Warning: posix_kill() expects parameter 1 to be integer, array given in %s on line %d
bool(false)

Notice: Array to string conversion in %sposix_kill_variation1.php on line %d

Arg value Array 

Warning: posix_kill() expects parameter 1 to be integer, array given in %s on line %d
bool(false)

Notice: Array to string conversion in %sposix_kill_variation1.php on line %d

Arg value Array 

Warning: posix_kill() expects parameter 1 to be integer, array given in %s on line %d
bool(false)

Notice: Array to string conversion in %sposix_kill_variation1.php on line %d

Arg value Array 

Warning: posix_kill() expects parameter 1 to be integer, array given in %s on line %d
bool(false)

Arg value  
bool(false)

Arg value  
bool(false)

Arg value 1 
bool(false)

Arg value  
bool(false)

Arg value 1 
bool(false)

Arg value  
bool(false)

Arg value  

Warning: posix_kill() expects parameter 1 to be integer, string given in %s on line %d
bool(false)

Arg value  

Warning: posix_kill() expects parameter 1 to be integer, string given in %s on line %d
bool(false)

Arg value string 

Warning: posix_kill() expects parameter 1 to be integer, string given in %s on line %d
bool(false)

Arg value string 

Warning: posix_kill() expects parameter 1 to be integer, string given in %s on line %d
bool(false)

Arg value  
bool(false)

Arg value  
bool(false)

Catchable fatal error: Object of class stdClass could not be converted to string in %s on line %d
