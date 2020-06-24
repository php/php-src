--TEST--
Test array_combine() function : usage variations - associative array with different values(Bug#43424)
--FILE--
<?php
/*
* Testing the functionality of array_combine() by passing various
* associative arrays having different possible values to $keys argument and
* associative arrays having different possible values to $values argument.
*/

echo "*** Testing array_combine() : assoc array with diff values to both \$keys and \$values argument ***\n";

// get an unset variable
$unset_var = 10;
unset ($unset_var);

// get a resource variable
$fp = fopen(__FILE__, "r");

// get a class
class classA
{
public function __toString(){
return "Class A object";
}
}

// get a heredoc string
$heredoc = <<<EOT
Hello world
EOT;

// different variations of associative array
$arrays = array (

       // empty array
/*1*/  array(),

       // arrays with integer values
       array('0' => 0),
       array("1" => 1),
       array("one" => 1, 'two' => 2, "three" => 3, 4 => 4),

       // arrays with float values
/*5*/  array("float" => 2.3333),
       array("f1" => 1.2, 'f2' => 3.33, 3 => 4.89999922839999, 'f4' => 33333333.333),

       // arrays with string values
/*7*/  array(111 => "\tHello", "red" => "col\tor", 2 => "\v\fworld", 3.3 =>  "pen\n"),
       array(111 => '\tHello', "red" => 'col\tor', 2 => '\v\fworld', 3.3 =>  'pen\n'),
       array(1 => "hello", "heredoc" => $heredoc),

       // array with object, unset variable and resource variable
/*10*/ array(11 => new classA(), "unset" => @$unset_var, "resource" => $fp),

       // array with mixed values
/*11*/ array(1 => 'hello', 2 => new classA(), 222 => "fruit",
             'resource' => $fp, "int" => 133, "float" => 444.432,
             "unset" => @$unset_var, "heredoc" => $heredoc)
);


// loop through each sub-array within $arrays to check the behavior of array_combine()
$iterator = 1;
foreach($arrays as $array) {
  echo "-- Iteration $iterator --\n";
  var_dump( array_combine($array, $array) );
  $iterator++;
}

// close the file resource used
fclose($fp);

echo "Done";
?>
--EXPECTF--
*** Testing array_combine() : assoc array with diff values to both $keys and $values argument ***
-- Iteration 1 --
array(0) {
}
-- Iteration 2 --
array(1) {
  [0]=>
  int(0)
}
-- Iteration 3 --
array(1) {
  [1]=>
  int(1)
}
-- Iteration 4 --
array(4) {
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(3)
  [4]=>
  int(4)
}
-- Iteration 5 --
array(1) {
  ["2.3333"]=>
  float(2.3333)
}
-- Iteration 6 --
array(4) {
  ["1.2"]=>
  float(1.2)
  ["3.33"]=>
  float(3.33)
  ["4.8999992284"]=>
  float(4.89999922839999)
  ["33333333.333"]=>
  float(33333333.333)
}
-- Iteration 7 --
array(4) {
  ["	Hello"]=>
  string(6) "	Hello"
  ["col	or"]=>
  string(6) "col	or"
  ["world"]=>
  string(7) "world"
  ["pen
"]=>
  string(4) "pen
"
}
-- Iteration 8 --
array(4) {
  ["\tHello"]=>
  string(7) "\tHello"
  ["col\tor"]=>
  string(7) "col\tor"
  ["\v\fworld"]=>
  string(9) "\v\fworld"
  ["pen\n"]=>
  string(5) "pen\n"
}
-- Iteration 9 --
array(2) {
  ["hello"]=>
  string(5) "hello"
  ["Hello world"]=>
  string(11) "Hello world"
}
-- Iteration 10 --
array(3) {
  ["Class A object"]=>
  object(classA)#%d (0) {
  }
  [""]=>
  NULL
  ["Resource id #%d"]=>
  resource(%d) of type (stream)
}
-- Iteration 11 --
array(8) {
  ["hello"]=>
  string(5) "hello"
  ["Class A object"]=>
  object(classA)#%d (0) {
  }
  ["fruit"]=>
  string(5) "fruit"
  ["Resource id #%d"]=>
  resource(%d) of type (stream)
  [133]=>
  int(133)
  ["444.432"]=>
  float(444.432)
  [""]=>
  NULL
  ["Hello world"]=>
  string(11) "Hello world"
}
Done
