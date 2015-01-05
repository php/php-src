--TEST--
Test array_unique() function : usage variations - associative array with different values
--FILE--
<?php
/* Prototype  : array array_unique(array $input)
 * Description: Removes duplicate values from array 
 * Source code: ext/standard/array.c
*/

/*
 * Testing the functionality of array_unique() by passing different
 * associative arrays having different values to $input argument.
*/

echo "*** Testing array_unique() : assoc. array with diff. values to \$input argument ***\n";

// get an unset variable
$unset_var = 10;
unset ($unset_var);

// get a resource variable
$fp = fopen(__FILE__, "r");

// get a class
class classA
{
  public function __toString() {
     return "Class A object";
  }
}

// get a heredoc string
$heredoc = <<<EOT
Hello world
EOT;

// associative arrays with different values
$inputs = array (
       // arrays with integer values
/*1*/  array('0' => 0, '1' => 0),
       array("one" => 1, 'two' => 2, "three" => 1, 4 => 1),

       // arrays with float values
/*3*/  array("float1" => 2.3333, "float2" => 2.3333),
       array("f1" => 1.2, 'f2' => 3.33, 3 => 4.89999922839999, 'f4' => 1.2),

       // arrays with string values
/*5*/  array(111 => "\tHello", "red" => "col\tor", 2 => "\v\fworld", 3.3 =>  "\tHello"),
       array(111 => '\tHello', "red" => 'col\tor', 2 => '\v\fworld', 3.3 =>  '\tHello'),
       array(1 => "hello", "heredoc" => $heredoc, $heredoc),

       // array with object, unset variable and resource variable
/*8*/ array(11 => new classA(), "unset" => @$unset_var, "resource" => $fp, new classA(), $fp),
);

// loop through each sub-array of $inputs to check the behavior of array_unique()
$iterator = 1;
foreach($inputs as $input) {
  echo "-- Iteration $iterator --\n";
  var_dump( array_unique($input) );
  $iterator++;
}

fclose($fp);
  
echo "Done";
?>
--EXPECTF--
*** Testing array_unique() : assoc. array with diff. values to $input argument ***
-- Iteration 1 --
array(1) {
  [0]=>
  int(0)
}
-- Iteration 2 --
array(2) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}
-- Iteration 3 --
array(1) {
  ["float1"]=>
  float(2.3333)
}
-- Iteration 4 --
array(3) {
  ["f1"]=>
  float(1.2)
  ["f2"]=>
  float(3.33)
  [3]=>
  float(4.8999992284)
}
-- Iteration 5 --
array(3) {
  [111]=>
  string(6) "	Hello"
  ["red"]=>
  string(6) "col	or"
  [2]=>
  string(7) "world"
}
-- Iteration 6 --
array(3) {
  [111]=>
  string(7) "\tHello"
  ["red"]=>
  string(7) "col\tor"
  [2]=>
  string(9) "\v\fworld"
}
-- Iteration 7 --
array(2) {
  [1]=>
  string(5) "hello"
  ["heredoc"]=>
  string(11) "Hello world"
}
-- Iteration 8 --
array(3) {
  [11]=>
  object(classA)#%d (0) {
  }
  ["unset"]=>
  NULL
  ["resource"]=>
  resource(%d) of type (stream)
}
Done
