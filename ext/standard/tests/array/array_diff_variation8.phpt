--TEST--
Test array_diff() function : usage variations - associative arrays contianing different data types
--FILE--
<?php
/* Prototype  : array array_diff(array $arr1, array $arr2 [, array ...])
 * Description: Returns the entries of $arr1 that have values which are
 * not present in any of the others arguments. 
 * Source code: ext/standard/array.c
 */

/*
 * Test array_diff() with associative arrays containing different data types as values
 */

echo "*** Testing array_diff() : usage variations ***\n";

$array = array('a' => '1', 'b' => '2', 'c' => '3');

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
  var_dump( array_diff($array, $input) );
  var_dump( array_diff($input, $array) );
  $iterator++;
}

fclose($fp);

echo "Done";
?>
--EXPECTF--
*** Testing array_diff() : usage variations ***
-- Iteration 1 --
array(3) {
  [u"a"]=>
  unicode(1) "1"
  [u"b"]=>
  unicode(1) "2"
  [u"c"]=>
  unicode(1) "3"
}
array(2) {
  [0]=>
  int(0)
  [1]=>
  int(0)
}
-- Iteration 2 --
array(1) {
  [u"c"]=>
  unicode(1) "3"
}
array(0) {
}
-- Iteration 3 --
array(3) {
  [u"a"]=>
  unicode(1) "1"
  [u"b"]=>
  unicode(1) "2"
  [u"c"]=>
  unicode(1) "3"
}
array(2) {
  [u"float1"]=>
  float(2.3333)
  [u"float2"]=>
  float(2.3333)
}
-- Iteration 4 --
array(3) {
  [u"a"]=>
  unicode(1) "1"
  [u"b"]=>
  unicode(1) "2"
  [u"c"]=>
  unicode(1) "3"
}
array(4) {
  [u"f1"]=>
  float(1.2)
  [u"f2"]=>
  float(3.33)
  [3]=>
  float(4.8999992284)
  [u"f4"]=>
  float(1.2)
}
-- Iteration 5 --
array(3) {
  [u"a"]=>
  unicode(1) "1"
  [u"b"]=>
  unicode(1) "2"
  [u"c"]=>
  unicode(1) "3"
}
array(4) {
  [111]=>
  unicode(6) "	Hello"
  [u"red"]=>
  unicode(6) "col	or"
  [2]=>
  unicode(7) "world"
  [3]=>
  unicode(6) "	Hello"
}
-- Iteration 6 --
array(3) {
  [u"a"]=>
  unicode(1) "1"
  [u"b"]=>
  unicode(1) "2"
  [u"c"]=>
  unicode(1) "3"
}
array(4) {
  [111]=>
  unicode(7) "\tHello"
  [u"red"]=>
  unicode(7) "col\tor"
  [2]=>
  unicode(9) "\v\fworld"
  [3]=>
  unicode(7) "\tHello"
}
-- Iteration 7 --
array(3) {
  [u"a"]=>
  unicode(1) "1"
  [u"b"]=>
  unicode(1) "2"
  [u"c"]=>
  unicode(1) "3"
}
array(3) {
  [1]=>
  unicode(5) "hello"
  [u"heredoc"]=>
  unicode(11) "Hello world"
  [2]=>
  unicode(11) "Hello world"
}
-- Iteration 8 --
array(3) {
  [u"a"]=>
  unicode(1) "1"
  [u"b"]=>
  unicode(1) "2"
  [u"c"]=>
  unicode(1) "3"
}
array(5) {
  [11]=>
  object(classA)#%d (0) {
  }
  [u"unset"]=>
  NULL
  [u"resource"]=>
  resource(%d) of type (stream)
  [12]=>
  object(classA)#%d (0) {
  }
  [13]=>
  resource(5) of type (stream)
}
Done
