--TEST--
Test crc32() function : usage variations - unexpected values
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) 
  die("skip this test is for 32bit platform only");
?>

--FILE--
<?php
/* Prototype  : string crc32(string $str)
 * Description: Calculate the crc32 polynomial of a string 
 * Source code: ext/standard/crc32.c
 * Alias to functions: none
*/

/*
 * Testing crc32() : with unexpected values for str argument
*/

echo "*** Testing crc32() : with unexpected values for str argument ***\n";

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
      new sample(),

      // undefined data
      $undefined_var,

      // unset data
      $unset_var,

      // resource
      $file_handle
);

// loop through each element of the array for str

$count = 1;
foreach($values as $value) {
      echo "\n-- Iteration $count --\n";
      var_dump( crc32($value) );
};

// closing the resource
fclose($file_handle);

echo "Done";
?>
--EXPECTF--
*** Testing crc32() : with unexpected values for str argument ***

Notice: Undefined variable: undefined_var in %s on line %d

Notice: Undefined variable: unset_var in %s on line %d

-- Iteration 1 --
int(-186917087)

-- Iteration 1 --
int(-2082672713)

-- Iteration 1 --
int(-873121252)

-- Iteration 1 --
int(1860518047)

-- Iteration 1 --
int(269248583)

-- Iteration 1 --
int(-834950157)

-- Iteration 1 --
int(-638440228)

-- Iteration 1 --
int(-742287383)

-- Iteration 1 --
int(-2036403827)

-- Iteration 1 --

Warning: crc32() expects parameter 1 to be string, array given in %s on line %d
NULL

-- Iteration 1 --

Warning: crc32() expects parameter 1 to be string, array given in %s on line %d
NULL

-- Iteration 1 --

Warning: crc32() expects parameter 1 to be string, array given in %s on line %d
NULL

-- Iteration 1 --

Warning: crc32() expects parameter 1 to be string, array given in %s on line %d
NULL

-- Iteration 1 --

Warning: crc32() expects parameter 1 to be string, array given in %s on line %d
NULL

-- Iteration 1 --
int(0)

-- Iteration 1 --
int(0)

-- Iteration 1 --
int(-2082672713)

-- Iteration 1 --
int(0)

-- Iteration 1 --
int(-2082672713)

-- Iteration 1 --
int(0)

-- Iteration 1 --
int(0)

-- Iteration 1 --
int(0)

-- Iteration 1 --
int(-1465013268)

-- Iteration 1 --
int(0)

-- Iteration 1 --
int(0)

-- Iteration 1 --

Warning: crc32() expects parameter 1 to be string, resource given in %s on line %d
NULL
Done
