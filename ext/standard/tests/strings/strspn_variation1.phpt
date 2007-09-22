--TEST--
Test strspn() function : usage variations  - unexpected values for str argument
--FILE--
<?php
/* Prototype  : proto int strspn(string str, string mask [, int start [, int len]])
 * Description: Finds length of initial segment consisting entirely of characters found in mask.
                If start or/and length is provided works like strspn(substr($s,$start,$len),$good_chars) 
 * Source code: ext/standard/string.c
 * Alias to functions: none
*/

/*
* Testing strspn() : with different unexpected values for str argument
*/

echo "*** Testing strspn() : with unexpected values for str argument ***\n";

// Initialise function arguments not being substititued (if any)
$mask = 'abons1234567890';
$start = 1;
$len = 10;


//get an unset variable
$unset_var = 10;
unset ($unset_var);

// declaring class
class sample  {
  public function __toString() {
    return "object";
  }
}

// creating a file resource
$file_handle = fopen(__FILE__, 'r');


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

      // object data
      new sample,

      // undefined data
      $undefined_var,

      // unset data
      $unset_var,

      // resource
      $file_handle
);

// loop through each element of the array for str

foreach($values as $value) {
      echo "\n-- Iteration with str value as \"$value\" \n";
      var_dump( strspn($value,$mask) ); // with default args
      var_dump( strspn($value,$mask,$start) );  // with default len value
      var_dump( strspn($value,$mask,$start,$len) );  //  with all args
};

// closing the resource
fclose($file_handle);

echo "Done"
?>
--EXPECTF--
*** Testing strspn() : with unexpected values for str argument ***

Notice: Undefined variable: undefined_var in %s on line %d

Notice: Undefined variable: unset_var in %s on line %d

-- Iteration with str value as "0" 
int(1)
int(0)
int(0)

-- Iteration with str value as "1" 
int(1)
int(0)
int(0)

-- Iteration with str value as "12345" 
int(5)
int(4)
int(4)

-- Iteration with str value as "-2345" 
int(0)
int(4)
int(4)

-- Iteration with str value as "10.5" 
int(2)
int(1)
int(1)

-- Iteration with str value as "-10.5" 
int(0)
int(2)
int(2)

-- Iteration with str value as "105000000000" 
int(12)
int(11)
int(10)

-- Iteration with str value as "1.06E-9" 
int(1)
int(0)
int(0)

-- Iteration with str value as "0.5" 
int(1)
int(0)
int(0)

-- Iteration with str value as "Array" 

Warning: strspn() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: strspn() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: strspn() expects parameter 1 to be string, array given in %s on line %d
NULL

-- Iteration with str value as "Array" 

Warning: strspn() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: strspn() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: strspn() expects parameter 1 to be string, array given in %s on line %d
NULL

-- Iteration with str value as "Array" 

Warning: strspn() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: strspn() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: strspn() expects parameter 1 to be string, array given in %s on line %d
NULL

-- Iteration with str value as "Array" 

Warning: strspn() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: strspn() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: strspn() expects parameter 1 to be string, array given in %s on line %d
NULL

-- Iteration with str value as "Array" 

Warning: strspn() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: strspn() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: strspn() expects parameter 1 to be string, array given in %s on line %d
NULL

-- Iteration with str value as "" 
int(0)
bool(false)
bool(false)

-- Iteration with str value as "" 
int(0)
bool(false)
bool(false)

-- Iteration with str value as "1" 
int(1)
int(0)
int(0)

-- Iteration with str value as "" 
int(0)
bool(false)
bool(false)

-- Iteration with str value as "1" 
int(1)
int(0)
int(0)

-- Iteration with str value as "" 
int(0)
bool(false)
bool(false)

-- Iteration with str value as "" 
int(0)
bool(false)
bool(false)

-- Iteration with str value as "" 
int(0)
bool(false)
bool(false)

-- Iteration with str value as "object" 
int(2)
int(1)
int(1)

-- Iteration with str value as "" 
int(0)
bool(false)
bool(false)

-- Iteration with str value as "" 
int(0)
bool(false)
bool(false)

-- Iteration with str value as "Resource id #%d" 

Warning: strspn() expects parameter 1 to be string, resource given in %s on line %d
NULL

Warning: strspn() expects parameter 1 to be string, resource given in %s on line %d
NULL

Warning: strspn() expects parameter 1 to be string, resource given in %s on line %d
NULL
Done
