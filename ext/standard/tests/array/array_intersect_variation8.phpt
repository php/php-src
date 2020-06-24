--TEST--
Test array_intersect() function : usage variations - assoc array with diff values for 'arr2' argument
--FILE--
<?php
/*
 * Testing the functionality of array_intersect() by passing different
 * associative arrays having different possible values to $arr2 argument.
 * The $arr1 argument is a fixed array.
*/

echo "*** Testing array_intersect() : assoc array with diff values to \$arr2 argument ***\n";

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

// different variations of associative arrays to be passed to $arr2 argument
$arrays = array (

       // empty array
/*1*/  array(),

       // arrays with integer values
       array('0' => 0),
       array("1" => 1),
       array("one" => 1, 'two' => 2, "three" => 3, 4 => 4),

       // arrays with float values
/*5*/  array("float" => 2.3333),
       array("f1" => 1.2, 'f2' => 3.33, 3 => 4.89999922839999, 'f4' => 33333333.333333),

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

// array to be passsed to $arr1 argument
$arr1 = array(1, 2, 1.2, 2.3333, "col\tor", '\v\fworld', $fp,
              "Hello world", $heredoc, new classA(), 444.432, "fruit");

// loop through each sub-array within $arrays to check the behavior of array_intersect()
$iterator = 1;
foreach($arrays as $arr2) {
  echo "-- Iterator $iterator --\n";

  // Calling array_intersect() with default arguments
  var_dump( array_intersect($arr1, $arr2) );

  // Calling array_intersect() with more arguments.
  // additional argument passed is the same as $arr1 argument
  var_dump( array_intersect($arr1, $arr2, $arr1) );
  $iterator++;
}

// close the file resource used
fclose($fp);

echo "Done";
?>
--EXPECTF--
*** Testing array_intersect() : assoc array with diff values to $arr2 argument ***
-- Iterator 1 --
array(0) {
}
array(0) {
}
-- Iterator 2 --
array(0) {
}
array(0) {
}
-- Iterator 3 --
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(1)
}
-- Iterator 4 --
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
-- Iterator 5 --
array(1) {
  [3]=>
  float(2.3333)
}
array(1) {
  [3]=>
  float(2.3333)
}
-- Iterator 6 --
array(1) {
  [2]=>
  float(1.2)
}
array(1) {
  [2]=>
  float(1.2)
}
-- Iterator 7 --
array(1) {
  [4]=>
  string(6) "col	or"
}
array(1) {
  [4]=>
  string(6) "col	or"
}
-- Iterator 8 --
array(1) {
  [5]=>
  string(9) "\v\fworld"
}
array(1) {
  [5]=>
  string(9) "\v\fworld"
}
-- Iterator 9 --
array(2) {
  [7]=>
  string(11) "Hello world"
  [8]=>
  string(11) "Hello world"
}
array(2) {
  [7]=>
  string(11) "Hello world"
  [8]=>
  string(11) "Hello world"
}
-- Iterator 10 --
array(2) {
  [6]=>
  resource(%d) of type (stream)
  [9]=>
  object(classA)#%d (0) {
  }
}
array(2) {
  [6]=>
  resource(%d) of type (stream)
  [9]=>
  object(classA)#%d (0) {
  }
}
-- Iterator 11 --
array(6) {
  [6]=>
  resource(%d) of type (stream)
  [7]=>
  string(11) "Hello world"
  [8]=>
  string(11) "Hello world"
  [9]=>
  object(classA)#%d (0) {
  }
  [10]=>
  float(444.432)
  [11]=>
  string(5) "fruit"
}
array(6) {
  [6]=>
  resource(%d) of type (stream)
  [7]=>
  string(11) "Hello world"
  [8]=>
  string(11) "Hello world"
  [9]=>
  object(classA)#%d (0) {
  }
  [10]=>
  float(444.432)
  [11]=>
  string(5) "fruit"
}
Done
