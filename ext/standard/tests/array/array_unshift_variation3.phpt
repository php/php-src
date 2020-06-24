--TEST--
Test array_unshift() function : usage variations - different array values for 'array' argument
--FILE--
<?php
/*
 * Testing the behavior of array_unshift() by passing different types of arrays
 * to $array argument to which the $var arguments will be prepended
*/

echo "*** Testing array_unshift() : different arrays for \$array argument ***\n";

// initialize $var argument
$var = 10;

// different arrays to be passed to $array argument
$arrays = array (
/*1*/  array(1, 2), // array with default keys and numeric values
       array(1.1, 2.2), // array with default keys & float values
       array( array(2), array(1)), // sub arrays
       array(false,true), // array with default keys and boolean values
       array(), // empty array
       array(NULL), // array with NULL
       array("a","aaaa","b","bbbb","c","ccccc"),

       // associative arrays
/*8*/  array(1 => "one", 2 => "two", 3 => "three"),  // explicit numeric keys, string values
       array("one" => 1, "two" => 2, "three" => 3 ),  // string keys & numeric values
       array( 1 => 10, 2 => 20, 4 => 40, 3 => 30),  // explicit numeric keys and numeric values
       array( "one" => "ten", "two" => "twenty", "three" => "thirty"),  // string key/value
       array("one" => 1, 2 => "two", 4 => "four"),  //mixed

       // associative array, containing null/empty/boolean values as key/value
/*13*/ array(NULL => "NULL", null => "null", "NULL" => NULL, "null" => null),
       array(true => "true", false => "false", "false" => false, "true" => true),
       array("" => "emptyd", '' => 'emptys', "emptyd" => "", 'emptys' => ''),
       array(1 => '', 2 => "", 3 => NULL, 4 => null, 5 => false, 6 => true),
       array('' => 1, "" => 2, NULL => 3, null => 4, false => 5, true => 6),

       // array with repetitive keys
/*18*/ array("One" => 1, "two" => 2, "One" => 10, "two" => 20, "three" => 3)
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
--EXPECT--
*** Testing array_unshift() : different arrays for $array argument ***
-- Iteration 1 --
int(3)
array(3) {
  [0]=>
  int(10)
  [1]=>
  int(1)
  [2]=>
  int(2)
}
int(5)
array(5) {
  [0]=>
  int(10)
  [1]=>
  string(5) "hello"
  [2]=>
  string(5) "world"
  [3]=>
  int(1)
  [4]=>
  int(2)
}
-- Iteration 2 --
int(3)
array(3) {
  [0]=>
  int(10)
  [1]=>
  float(1.1)
  [2]=>
  float(2.2)
}
int(5)
array(5) {
  [0]=>
  int(10)
  [1]=>
  string(5) "hello"
  [2]=>
  string(5) "world"
  [3]=>
  float(1.1)
  [4]=>
  float(2.2)
}
-- Iteration 3 --
int(3)
array(3) {
  [0]=>
  int(10)
  [1]=>
  array(1) {
    [0]=>
    int(2)
  }
  [2]=>
  array(1) {
    [0]=>
    int(1)
  }
}
int(5)
array(5) {
  [0]=>
  int(10)
  [1]=>
  string(5) "hello"
  [2]=>
  string(5) "world"
  [3]=>
  array(1) {
    [0]=>
    int(2)
  }
  [4]=>
  array(1) {
    [0]=>
    int(1)
  }
}
-- Iteration 4 --
int(3)
array(3) {
  [0]=>
  int(10)
  [1]=>
  bool(false)
  [2]=>
  bool(true)
}
int(5)
array(5) {
  [0]=>
  int(10)
  [1]=>
  string(5) "hello"
  [2]=>
  string(5) "world"
  [3]=>
  bool(false)
  [4]=>
  bool(true)
}
-- Iteration 5 --
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
  string(5) "hello"
  [2]=>
  string(5) "world"
}
-- Iteration 6 --
int(2)
array(2) {
  [0]=>
  int(10)
  [1]=>
  NULL
}
int(4)
array(4) {
  [0]=>
  int(10)
  [1]=>
  string(5) "hello"
  [2]=>
  string(5) "world"
  [3]=>
  NULL
}
-- Iteration 7 --
int(7)
array(7) {
  [0]=>
  int(10)
  [1]=>
  string(1) "a"
  [2]=>
  string(4) "aaaa"
  [3]=>
  string(1) "b"
  [4]=>
  string(4) "bbbb"
  [5]=>
  string(1) "c"
  [6]=>
  string(5) "ccccc"
}
int(9)
array(9) {
  [0]=>
  int(10)
  [1]=>
  string(5) "hello"
  [2]=>
  string(5) "world"
  [3]=>
  string(1) "a"
  [4]=>
  string(4) "aaaa"
  [5]=>
  string(1) "b"
  [6]=>
  string(4) "bbbb"
  [7]=>
  string(1) "c"
  [8]=>
  string(5) "ccccc"
}
-- Iteration 8 --
int(4)
array(4) {
  [0]=>
  int(10)
  [1]=>
  string(3) "one"
  [2]=>
  string(3) "two"
  [3]=>
  string(5) "three"
}
int(6)
array(6) {
  [0]=>
  int(10)
  [1]=>
  string(5) "hello"
  [2]=>
  string(5) "world"
  [3]=>
  string(3) "one"
  [4]=>
  string(3) "two"
  [5]=>
  string(5) "three"
}
-- Iteration 9 --
int(4)
array(4) {
  [0]=>
  int(10)
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
  ["three"]=>
  int(3)
}
int(6)
array(6) {
  [0]=>
  int(10)
  [1]=>
  string(5) "hello"
  [2]=>
  string(5) "world"
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
  ["three"]=>
  int(3)
}
-- Iteration 10 --
int(5)
array(5) {
  [0]=>
  int(10)
  [1]=>
  int(10)
  [2]=>
  int(20)
  [3]=>
  int(40)
  [4]=>
  int(30)
}
int(7)
array(7) {
  [0]=>
  int(10)
  [1]=>
  string(5) "hello"
  [2]=>
  string(5) "world"
  [3]=>
  int(10)
  [4]=>
  int(20)
  [5]=>
  int(40)
  [6]=>
  int(30)
}
-- Iteration 11 --
int(4)
array(4) {
  [0]=>
  int(10)
  ["one"]=>
  string(3) "ten"
  ["two"]=>
  string(6) "twenty"
  ["three"]=>
  string(6) "thirty"
}
int(6)
array(6) {
  [0]=>
  int(10)
  [1]=>
  string(5) "hello"
  [2]=>
  string(5) "world"
  ["one"]=>
  string(3) "ten"
  ["two"]=>
  string(6) "twenty"
  ["three"]=>
  string(6) "thirty"
}
-- Iteration 12 --
int(4)
array(4) {
  [0]=>
  int(10)
  ["one"]=>
  int(1)
  [1]=>
  string(3) "two"
  [2]=>
  string(4) "four"
}
int(6)
array(6) {
  [0]=>
  int(10)
  [1]=>
  string(5) "hello"
  [2]=>
  string(5) "world"
  ["one"]=>
  int(1)
  [3]=>
  string(3) "two"
  [4]=>
  string(4) "four"
}
-- Iteration 13 --
int(4)
array(4) {
  [0]=>
  int(10)
  [""]=>
  string(4) "null"
  ["NULL"]=>
  NULL
  ["null"]=>
  NULL
}
int(6)
array(6) {
  [0]=>
  int(10)
  [1]=>
  string(5) "hello"
  [2]=>
  string(5) "world"
  [""]=>
  string(4) "null"
  ["NULL"]=>
  NULL
  ["null"]=>
  NULL
}
-- Iteration 14 --
int(5)
array(5) {
  [0]=>
  int(10)
  [1]=>
  string(4) "true"
  [2]=>
  string(5) "false"
  ["false"]=>
  bool(false)
  ["true"]=>
  bool(true)
}
int(7)
array(7) {
  [0]=>
  int(10)
  [1]=>
  string(5) "hello"
  [2]=>
  string(5) "world"
  [3]=>
  string(4) "true"
  [4]=>
  string(5) "false"
  ["false"]=>
  bool(false)
  ["true"]=>
  bool(true)
}
-- Iteration 15 --
int(4)
array(4) {
  [0]=>
  int(10)
  [""]=>
  string(6) "emptys"
  ["emptyd"]=>
  string(0) ""
  ["emptys"]=>
  string(0) ""
}
int(6)
array(6) {
  [0]=>
  int(10)
  [1]=>
  string(5) "hello"
  [2]=>
  string(5) "world"
  [""]=>
  string(6) "emptys"
  ["emptyd"]=>
  string(0) ""
  ["emptys"]=>
  string(0) ""
}
-- Iteration 16 --
int(7)
array(7) {
  [0]=>
  int(10)
  [1]=>
  string(0) ""
  [2]=>
  string(0) ""
  [3]=>
  NULL
  [4]=>
  NULL
  [5]=>
  bool(false)
  [6]=>
  bool(true)
}
int(9)
array(9) {
  [0]=>
  int(10)
  [1]=>
  string(5) "hello"
  [2]=>
  string(5) "world"
  [3]=>
  string(0) ""
  [4]=>
  string(0) ""
  [5]=>
  NULL
  [6]=>
  NULL
  [7]=>
  bool(false)
  [8]=>
  bool(true)
}
-- Iteration 17 --
int(4)
array(4) {
  [0]=>
  int(10)
  [""]=>
  int(4)
  [1]=>
  int(5)
  [2]=>
  int(6)
}
int(6)
array(6) {
  [0]=>
  int(10)
  [1]=>
  string(5) "hello"
  [2]=>
  string(5) "world"
  [""]=>
  int(4)
  [3]=>
  int(5)
  [4]=>
  int(6)
}
-- Iteration 18 --
int(4)
array(4) {
  [0]=>
  int(10)
  ["One"]=>
  int(10)
  ["two"]=>
  int(20)
  ["three"]=>
  int(3)
}
int(6)
array(6) {
  [0]=>
  int(10)
  [1]=>
  string(5) "hello"
  [2]=>
  string(5) "world"
  ["One"]=>
  int(10)
  ["two"]=>
  int(20)
  ["three"]=>
  int(3)
}
Done
