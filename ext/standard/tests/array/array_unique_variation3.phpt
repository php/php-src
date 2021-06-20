--TEST--
Test array_unique() function : usage variations - associative array with different keys
--FILE--
<?php
/*
 * Testing the functionality of array_unique() by passing different
 * associative arrays having different keys to $input argument.
*/

echo "*** Testing array_unique() : assoc. array with diff. keys passed to \$input argument ***\n";

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

// different associative arrays to be passed to $input argument
$inputs = array (
/*1*/  // arrays with integer keys
       array(0 => "0", 1 => "0"),
       array(1 => "1", 2 => "2", 3 => 1, 4 => "4"),

       // arrays with string keys
/*5*/  array('\tHello' => 111, 're\td' => "color", '\v\fworld' => 2.2, 'pen\n' => 111),
       array("\tHello" => 111, "re\td" => "color", "\v\fworld" => 2.2, "pen\n" => 111),
       array("hello", $heredoc => "string", "string"),

       // array with object, unset variable and resource variable
/*8*/ array(@$unset_var => "hello", $fp => 'resource', 11, "hello"),
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
*** Testing array_unique() : assoc. array with diff. keys passed to $input argument ***

Warning: Resource ID#%d used as offset, casting to integer (%d) in %s on line %d
-- Iteration 1 --
array(1) {
  [0]=>
  string(1) "0"
}
-- Iteration 2 --
array(3) {
  [1]=>
  string(1) "1"
  [2]=>
  string(1) "2"
  [4]=>
  string(1) "4"
}
-- Iteration 3 --
array(3) {
  ["\tHello"]=>
  int(111)
  ["re\td"]=>
  string(5) "color"
  ["\v\fworld"]=>
  float(2.2)
}
-- Iteration 4 --
array(3) {
  ["	Hello"]=>
  int(111)
  ["re	d"]=>
  string(5) "color"
  ["world"]=>
  float(2.2)
}
-- Iteration 5 --
array(2) {
  [0]=>
  string(5) "hello"
  ["Hello world"]=>
  string(6) "string"
}
-- Iteration 6 --
array(3) {
  [""]=>
  string(5) "hello"
  [5]=>
  string(8) "resource"
  [6]=>
  int(11)
}
Done
