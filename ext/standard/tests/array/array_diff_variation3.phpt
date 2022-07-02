--TEST--
Test array_diff() function : usage variations  - array with different data types as values
--FILE--
<?php
/*
 * Test how array_diff() compares indexed arrays containing different
 * data types as values in place of $arr1
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

// loop through each element of the array for arr1
$iterator = 1;
foreach($values as $value) {
      echo "\n Iteration: $iterator \n";
      var_dump( array_diff($value, $array) );
      $iterator++;
};

echo "Done";
?>
--EXPECT--
*** Testing array_diff() : usage variations ***

 Iteration: 1 
array(0) {
}

 Iteration: 2 
array(3) {
  [0]=>
  int(0)
  [2]=>
  int(12345)
  [3]=>
  int(-2345)
}

 Iteration: 3 
array(5) {
  [0]=>
  float(10.5)
  [1]=>
  float(-10.5)
  [2]=>
  float(123456789000)
  [3]=>
  float(1.23456789E-9)
  [4]=>
  float(0.5)
}

 Iteration: 4 
array(2) {
  [0]=>
  NULL
  [1]=>
  NULL
}

 Iteration: 5 
array(2) {
  [1]=>
  bool(false)
  [3]=>
  bool(false)
}

 Iteration: 6 
array(2) {
  [0]=>
  string(0) ""
  [1]=>
  string(0) ""
}

 Iteration: 7 
array(3) {
  [0]=>
  string(6) "string"
  [1]=>
  string(6) "string"
  [2]=>
  string(17) "This is a heredoc"
}

 Iteration: 8 
array(2) {
  [0]=>
  string(6) "binary"
  [1]=>
  string(6) "binary"
}

 Iteration: 9 
array(1) {
  [0]=>
  NULL
}

 Iteration: 10 
array(1) {
  [0]=>
  NULL
}
Done
