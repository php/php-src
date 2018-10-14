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
      $count++;
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

-- Iteration 2 --
int(-2082672713)

-- Iteration 3 --
int(-873121252)

-- Iteration 4 --
int(1860518047)

-- Iteration 5 --
int(269248583)

-- Iteration 6 --
int(-834950157)

-- Iteration 7 --
int(-965354630)

-- Iteration 8 --
int(1376932222)

-- Iteration 9 --
int(-2036403827)

-- Iteration 10 --

Warning: crc32() expects parameter 1 to be string, array given in %s on line %d
NULL

-- Iteration 11 --

Warning: crc32() expects parameter 1 to be string, array given in %s on line %d
NULL

-- Iteration 12 --

Warning: crc32() expects parameter 1 to be string, array given in %s on line %d
NULL

-- Iteration 13 --

Warning: crc32() expects parameter 1 to be string, array given in %s on line %d
NULL

-- Iteration 14 --

Warning: crc32() expects parameter 1 to be string, array given in %s on line %d
NULL

-- Iteration 15 --
int(0)

-- Iteration 16 --
int(0)

-- Iteration 17 --
int(-2082672713)

-- Iteration 18 --
int(0)

-- Iteration 19 --
int(-2082672713)

-- Iteration 20 --
int(0)

-- Iteration 21 --
int(0)

-- Iteration 22 --
int(0)

-- Iteration 23 --
int(-1465013268)

-- Iteration 24 --
int(0)

-- Iteration 25 --
int(0)

-- Iteration 26 --

Warning: crc32() expects parameter 1 to be string, resource given in %s on line %d
NULL
Done
