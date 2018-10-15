--TEST--
Test array_diff() function : usage variations  - array with different data types as values
--FILE--
<?php
/* Prototype  : array array_diff(array $arr1, array $arr2 [, array ...])
 * Description: Returns the entries of $arr1 that have values which are not
 * present in any of the others arguments.
 * Source code: ext/standard/array.c
 */

/*
 * Test how array_diff() compares indexed arrays containing different
 * data types as values in place of $arr2
 */

echo "*** Testing array_diff() : usage variations ***\n";

// Initialise function arguments not being substituted (if any)
$array = array(1, 2);

//get an unset variable
$unset_var = 10;
unset ($unset_var);

//get heredoc
$heredoc = <<<END
This is a heredoc
END;

//array of values to iterate over
$values = array(

/*1*/"empty array" => array(),

/*2*/
"int" => array(
      // int data
      0,
      1,
      12345,
      -2345),

/*3*/
"float" => array(
      // float data
       10.5,
       -10.5,
       12.3456789000e10,
       12.3456789000E-10,
       .5),

/*4*/
"null" => array(
      // null data
      NULL,
      null),

/*5*/
"boolean" => array(
      // boolean data
      true,
      false,
      TRUE,
      FALSE),

/*6*/
"empty" => array(
      // empty data
      "",
      ''),

/*7*/
"string" => array(
      // string data
      "string",
      'string',
      $heredoc),

/*8*/
"binary" => array(
       // binary data
       b"binary",
	   (binary)"binary"),

/*9*/
"undefined" => array(
      // undefined data
      @$undefined_var),

/*10*/
"unset" => array(
      // unset data
      @$unset_var)
);

// loop through each element of the array for $arr2
$iterator = 1;
foreach($values as $value) {
      echo "\n Iteration: $iterator \n";
      var_dump( array_diff($array, $value) );
      $iterator++;
};

echo "Done";
?>
--EXPECTF--
*** Testing array_diff() : usage variations ***

 Iteration: 1 
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}

 Iteration: 2 
array(1) {
  [1]=>
  int(2)
}

 Iteration: 3 
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}

 Iteration: 4 
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}

 Iteration: 5 
array(1) {
  [1]=>
  int(2)
}

 Iteration: 6 
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}

 Iteration: 7 
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}

 Iteration: 8 
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}

 Iteration: 9 
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}

 Iteration: 10 
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
Done
