--TEST--
Test strcspn() function : usage variations - unexpected values of start argument
--FILE--
<?php
/* Prototype  : proto int strcspn(string str, string mask [,int start [,int len]])
 * Description: Finds length of initial segment consisting entirely of characters not found in mask.
                If start or/and length is provided works like strcspn(substr($s,$start,$len),$bad_chars) 
 * Source code: ext/standard/string.c
 * Alias to functions: none
*/

error_reporting(E_ALL & ~E_NOTICE);

/*
* Testing strcspn() : with unexpected values of start argument
*/

echo "*** Testing strcspn() : with unexpected values of start argument ***\n";

// initialing required variables
$str = 'string_val';
$mask = 'soibtFTf1234567890';
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

      // float data
      10.5,
      -10.5,
      10.1234567e8,
      10.7654321E-8,
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
      new sample(),

      // undefined data
      $undefined_var,

      // unset data
      $unset_var,

      // resource
      $file_handle
);

// loop through each element of the array for start

foreach($values as $value) {
      echo "\n-- Iteration with start value as \"$value\" --\n";
      var_dump( strcspn($str,$mask,$value) );  // with default len value
      var_dump( strcspn($str,$mask,$value,$len) );  // with all args
};

// closing the resource
fclose($file_handle);

echo "Done"
?>
--EXPECTF--
*** Testing strcspn() : with unexpected values of start argument ***

-- Iteration with start value as "10.5" --
int(0)
int(0)

-- Iteration with start value as "-10.5" --
int(0)
int(0)

-- Iteration with start value as "1012345670" --
bool(false)
bool(false)

-- Iteration with start value as "1.07654321E-7" --
int(0)
int(0)

-- Iteration with start value as "0.5" --
int(0)
int(0)

-- Iteration with start value as "Array" --

Warning: strcspn() expects parameter 3 to be integer, array given in %s on line %d
NULL

Warning: strcspn() expects parameter 3 to be integer, array given in %s on line %d
NULL

-- Iteration with start value as "Array" --

Warning: strcspn() expects parameter 3 to be integer, array given in %s on line %d
NULL

Warning: strcspn() expects parameter 3 to be integer, array given in %s on line %d
NULL

-- Iteration with start value as "Array" --

Warning: strcspn() expects parameter 3 to be integer, array given in %s on line %d
NULL

Warning: strcspn() expects parameter 3 to be integer, array given in %s on line %d
NULL

-- Iteration with start value as "Array" --

Warning: strcspn() expects parameter 3 to be integer, array given in %s on line %d
NULL

Warning: strcspn() expects parameter 3 to be integer, array given in %s on line %d
NULL

-- Iteration with start value as "Array" --

Warning: strcspn() expects parameter 3 to be integer, array given in %s on line %d
NULL

Warning: strcspn() expects parameter 3 to be integer, array given in %s on line %d
NULL

-- Iteration with start value as "" --
int(0)
int(0)

-- Iteration with start value as "" --
int(0)
int(0)

-- Iteration with start value as "1" --
int(0)
int(0)

-- Iteration with start value as "" --
int(0)
int(0)

-- Iteration with start value as "1" --
int(0)
int(0)

-- Iteration with start value as "" --
int(0)
int(0)

-- Iteration with start value as "" --

Warning: strcspn() expects parameter 3 to be integer, string given in %s on line %d
NULL

Warning: strcspn() expects parameter 3 to be integer, string given in %s on line %d
NULL

-- Iteration with start value as "" --

Warning: strcspn() expects parameter 3 to be integer, string given in %s on line %d
NULL

Warning: strcspn() expects parameter 3 to be integer, string given in %s on line %d
NULL

-- Iteration with start value as "string" --

Warning: strcspn() expects parameter 3 to be integer, string given in %s on line %d
NULL

Warning: strcspn() expects parameter 3 to be integer, string given in %s on line %d
NULL

-- Iteration with start value as "string" --

Warning: strcspn() expects parameter 3 to be integer, string given in %s on line %d
NULL

Warning: strcspn() expects parameter 3 to be integer, string given in %s on line %d
NULL

-- Iteration with start value as "object" --

Warning: strcspn() expects parameter 3 to be integer, object given in %s on line %d
NULL

Warning: strcspn() expects parameter 3 to be integer, object given in %s on line %d
NULL

-- Iteration with start value as "" --
int(0)
int(0)

-- Iteration with start value as "" --
int(0)
int(0)

-- Iteration with start value as "Resource id #%d" --

Warning: strcspn() expects parameter 3 to be integer, resource given in %s on line %d
NULL

Warning: strcspn() expects parameter 3 to be integer, resource given in %s on line %d
NULL
Done