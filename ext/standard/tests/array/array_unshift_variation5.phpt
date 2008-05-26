--TEST--
Test array_unshift() function : usage variations - assoc. array with diff values for 'array' argument
--INI--
precision=12
--FILE-- 
<?php
/* Prototype  : int array_unshift(array $array, mixed $var [, mixed ...])
 * Description: Pushes elements onto the beginning of the array
 * Source code: ext/standard/array.c
*/

/*
 * Testing the functionality of array_unshift() by passing different
 * associative arrays having different possible values to $array argument.
 * The $var argument passed is a fixed value
*/
 
echo "*** Testing array_unshift() : associative array with different values ***\n";

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

// initializing $var argument
$var = 10;

// different variations of associative arrays to be passed to $array argument
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
       array(11 => new classA(), "unset" => @$unset_var, "resource" => $fp),

       // array with mixed values
/*11*/ array(1 => 'hello', 2 => new classA(), 222 => "fruit", 'resource' => $fp, "int" => 133, "float" => 444.432, "unset" => @$unset_var, "heredoc" => $heredoc)
);

// loop through the various elements of $arrays to test array_unshift()
$iterator = 1;
foreach($arrays as $array) {
  echo "-- Iteration $iterator --\n";

  /* with default argument */
  // returns element count in the resulting array after arguments are pushed to
  // beginning of the given array 
  $temp_array = $array;
  var_dump( array_unshift($temp_array, $var) );

  // dump the resulting array
  var_dump($temp_array);

  /* with optional arguments */
  // returns element count in the resulting array after arguments are pushed to
  // beginning of the given array 
  $temp_array = $array;
  var_dump( array_unshift($temp_array, $var, "hello", 'world') );

  // dump the resulting array
  var_dump($temp_array);
  $iterator++;
}

echo "Done";
?>
--EXPECTF--
*** Testing array_unshift() : associative array with different values ***
-- Iteration 1 --
int(1)
array(1) {
  [0]=>
  int(10)
}
int(3)
array(3) {
  [0]=>
  int(10)
  [1]=>
  unicode(5) "hello"
  [2]=>
  unicode(5) "world"
}
-- Iteration 2 --
int(2)
array(2) {
  [0]=>
  int(10)
  [1]=>
  int(0)
}
int(4)
array(4) {
  [0]=>
  int(10)
  [1]=>
  unicode(5) "hello"
  [2]=>
  unicode(5) "world"
  [3]=>
  int(0)
}
-- Iteration 3 --
int(2)
array(2) {
  [0]=>
  int(10)
  [1]=>
  int(1)
}
int(4)
array(4) {
  [0]=>
  int(10)
  [1]=>
  unicode(5) "hello"
  [2]=>
  unicode(5) "world"
  [3]=>
  int(1)
}
-- Iteration 4 --
int(5)
array(5) {
  [0]=>
  int(10)
  [u"one"]=>
  int(1)
  [u"two"]=>
  int(2)
  [u"three"]=>
  int(3)
  [1]=>
  int(4)
}
int(7)
array(7) {
  [0]=>
  int(10)
  [1]=>
  unicode(5) "hello"
  [2]=>
  unicode(5) "world"
  [u"one"]=>
  int(1)
  [u"two"]=>
  int(2)
  [u"three"]=>
  int(3)
  [3]=>
  int(4)
}
-- Iteration 5 --
int(2)
array(2) {
  [0]=>
  int(10)
  [u"float"]=>
  float(2.3333)
}
int(4)
array(4) {
  [0]=>
  int(10)
  [1]=>
  unicode(5) "hello"
  [2]=>
  unicode(5) "world"
  [u"float"]=>
  float(2.3333)
}
-- Iteration 6 --
int(5)
array(5) {
  [0]=>
  int(10)
  [u"f1"]=>
  float(1.2)
  [u"f2"]=>
  float(3.33)
  [1]=>
  float(4.8999992284)
  [u"f4"]=>
  float(33333333.3333)
}
int(7)
array(7) {
  [0]=>
  int(10)
  [1]=>
  unicode(5) "hello"
  [2]=>
  unicode(5) "world"
  [u"f1"]=>
  float(1.2)
  [u"f2"]=>
  float(3.33)
  [3]=>
  float(4.8999992284)
  [u"f4"]=>
  float(33333333.3333)
}
-- Iteration 7 --
int(5)
array(5) {
  [0]=>
  int(10)
  [1]=>
  unicode(6) "	Hello"
  [u"red"]=>
  unicode(6) "col	or"
  [2]=>
  unicode(7) "world"
  [3]=>
  unicode(4) "pen
"
}
int(7)
array(7) {
  [0]=>
  int(10)
  [1]=>
  unicode(5) "hello"
  [2]=>
  unicode(5) "world"
  [3]=>
  unicode(6) "	Hello"
  [u"red"]=>
  unicode(6) "col	or"
  [4]=>
  unicode(7) "world"
  [5]=>
  unicode(4) "pen
"
}
-- Iteration 8 --
int(5)
array(5) {
  [0]=>
  int(10)
  [1]=>
  unicode(7) "\tHello"
  [u"red"]=>
  unicode(7) "col\tor"
  [2]=>
  unicode(9) "\v\fworld"
  [3]=>
  unicode(5) "pen\n"
}
int(7)
array(7) {
  [0]=>
  int(10)
  [1]=>
  unicode(5) "hello"
  [2]=>
  unicode(5) "world"
  [3]=>
  unicode(7) "\tHello"
  [u"red"]=>
  unicode(7) "col\tor"
  [4]=>
  unicode(9) "\v\fworld"
  [5]=>
  unicode(5) "pen\n"
}
-- Iteration 9 --
int(3)
array(3) {
  [0]=>
  int(10)
  [1]=>
  unicode(5) "hello"
  [u"heredoc"]=>
  unicode(11) "Hello world"
}
int(5)
array(5) {
  [0]=>
  int(10)
  [1]=>
  unicode(5) "hello"
  [2]=>
  unicode(5) "world"
  [3]=>
  unicode(5) "hello"
  [u"heredoc"]=>
  unicode(11) "Hello world"
}
-- Iteration 10 --
int(4)
array(4) {
  [0]=>
  int(10)
  [1]=>
  object(classA)#%d (0) {
  }
  [u"unset"]=>
  NULL
  [u"resource"]=>
  resource(%d) of type (stream)
}
int(6)
array(6) {
  [0]=>
  int(10)
  [1]=>
  unicode(5) "hello"
  [2]=>
  unicode(5) "world"
  [3]=>
  object(classA)#%d (0) {
  }
  [u"unset"]=>
  NULL
  [u"resource"]=>
  resource(%d) of type (stream)
}
-- Iteration 11 --
int(9)
array(9) {
  [0]=>
  int(10)
  [1]=>
  unicode(5) "hello"
  [2]=>
  object(classA)#%d (0) {
  }
  [3]=>
  unicode(5) "fruit"
  [u"resource"]=>
  resource(%d) of type (stream)
  [u"int"]=>
  int(133)
  [u"float"]=>
  float(444.432)
  [u"unset"]=>
  NULL
  [u"heredoc"]=>
  unicode(11) "Hello world"
}
int(11)
array(11) {
  [0]=>
  int(10)
  [1]=>
  unicode(5) "hello"
  [2]=>
  unicode(5) "world"
  [3]=>
  unicode(5) "hello"
  [4]=>
  object(classA)#%d (0) {
  }
  [5]=>
  unicode(5) "fruit"
  [u"resource"]=>
  resource(%d) of type (stream)
  [u"int"]=>
  int(133)
  [u"float"]=>
  float(444.432)
  [u"unset"]=>
  NULL
  [u"heredoc"]=>
  unicode(11) "Hello world"
}
Done
