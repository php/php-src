--TEST--
Test array_diff_assoc() function : usage variations - arrays with different data types as keys
--FILE--
<?php
/*
 * Test how array_diff_assoc() compares arrays containing different data types
 * as keys
 */

echo "\n*** Testing array_diff_assoc() : usage variations ***\n";

$array = array(1, 2, 3);

//get an unset variable
$unset_var = 10;
unset ($unset_var);

// heredoc string
$heredoc = <<<EOT
hello world
EOT;

//Different data types as keys to be passed to $arr1 argument
$inputs = array(

       // int data
/*1*/
'int' => array(
       0 => 'zero',
       1 => 'one',
       12345 => 'positive',
       -2345 => 'negative'),

       // null data
/*3*/
'null' => array(
       NULL => 'null 1',
       null => 'null 2'),

       // boolean data
/*4*/
'bool' => array(
       true => 'boolt',
       false => 'boolf',
       TRUE => 'boolT',
       FALSE => 'boolF'),

       // empty data
/*5*/
'empty' => array(
      "" => 'emptyd',
      '' => 'emptys'),

       // string data
/*6*/
'string' => array(
      "string" => 'stringd',
      'string' => 'strings',
      $heredoc => 'stringh'),

       // binary data
/*7*/
'binary' => array(
      b"binary1" => 'binary 1',
      (binary)"binary2" => 'binary 2'),

       // undefined data
/*8*/
'undefined' => array(
      @$undefined_var => 'undefined'),

       // unset data
/*9*/
'unset' => array(
      @$unset_var => 'unset'),

);

// loop through each element of $inputs to check the behavior of array_diff_assoc
$iterator = 1;
foreach($inputs as $key => $input) {
  echo "\n-- Iteration $iterator --\n";
  var_dump( array_diff_assoc($input, $array));
  $iterator++;
};

echo "Done";
?>
--EXPECT--
*** Testing array_diff_assoc() : usage variations ***

-- Iteration 1 --
array(4) {
  [0]=>
  string(4) "zero"
  [1]=>
  string(3) "one"
  [12345]=>
  string(8) "positive"
  [-2345]=>
  string(8) "negative"
}

-- Iteration 2 --
array(1) {
  [""]=>
  string(6) "null 2"
}

-- Iteration 3 --
array(2) {
  [1]=>
  string(5) "boolT"
  [0]=>
  string(5) "boolF"
}

-- Iteration 4 --
array(1) {
  [""]=>
  string(6) "emptys"
}

-- Iteration 5 --
array(2) {
  ["string"]=>
  string(7) "strings"
  ["hello world"]=>
  string(7) "stringh"
}

-- Iteration 6 --
array(2) {
  ["binary1"]=>
  string(8) "binary 1"
  ["binary2"]=>
  string(8) "binary 2"
}

-- Iteration 7 --
array(1) {
  [""]=>
  string(9) "undefined"
}

-- Iteration 8 --
array(1) {
  [""]=>
  string(5) "unset"
}
Done
