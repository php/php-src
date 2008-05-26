--TEST--
Test array_map() function : usage variations - different arrays for 'arr1' argument
--FILE--
<?php
/* Prototype  : array array_map  ( callback $callback  , array $arr1  [, array $...  ] )
 * Description: Applies the callback to the elements of the given arrays
 * Source code: ext/standard/array.c
 */

/*
 * Test array_map() by passing different arrays for $arr1 argument
 */

echo "*** Testing array_map() : different arrays for 'arr1' argument ***\n";

function callback($a)
{
  return ($a);
}

// different arrays
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

       // array with repetative keys
/*18*/ array("One" => 1, "two" => 2, "One" => 10, "two" => 20, "three" => 3)
);

// loop through the various elements of $arrays to test array_map()
$iterator = 1;
foreach($arrays as $arr1) {
  echo "-- Iteration $iterator --\n";
  var_dump( array_map('callback', $arr1) );
  $iterator++;
}

echo "Done";
?>
--EXPECT--
*** Testing array_map() : different arrays for 'arr1' argument ***
-- Iteration 1 --
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
-- Iteration 2 --
array(2) {
  [0]=>
  float(1.1)
  [1]=>
  float(2.2)
}
-- Iteration 3 --
array(2) {
  [0]=>
  array(1) {
    [0]=>
    int(2)
  }
  [1]=>
  array(1) {
    [0]=>
    int(1)
  }
}
-- Iteration 4 --
array(2) {
  [0]=>
  bool(false)
  [1]=>
  bool(true)
}
-- Iteration 5 --
array(0) {
}
-- Iteration 6 --
array(1) {
  [0]=>
  NULL
}
-- Iteration 7 --
array(6) {
  [0]=>
  unicode(1) "a"
  [1]=>
  unicode(4) "aaaa"
  [2]=>
  unicode(1) "b"
  [3]=>
  unicode(4) "bbbb"
  [4]=>
  unicode(1) "c"
  [5]=>
  unicode(5) "ccccc"
}
-- Iteration 8 --
array(3) {
  [1]=>
  unicode(3) "one"
  [2]=>
  unicode(3) "two"
  [3]=>
  unicode(5) "three"
}
-- Iteration 9 --
array(3) {
  [u"one"]=>
  int(1)
  [u"two"]=>
  int(2)
  [u"three"]=>
  int(3)
}
-- Iteration 10 --
array(4) {
  [1]=>
  int(10)
  [2]=>
  int(20)
  [4]=>
  int(40)
  [3]=>
  int(30)
}
-- Iteration 11 --
array(3) {
  [u"one"]=>
  unicode(3) "ten"
  [u"two"]=>
  unicode(6) "twenty"
  [u"three"]=>
  unicode(6) "thirty"
}
-- Iteration 12 --
array(3) {
  [u"one"]=>
  int(1)
  [2]=>
  unicode(3) "two"
  [4]=>
  unicode(4) "four"
}
-- Iteration 13 --
array(3) {
  [u""]=>
  unicode(4) "null"
  [u"NULL"]=>
  NULL
  [u"null"]=>
  NULL
}
-- Iteration 14 --
array(4) {
  [1]=>
  unicode(4) "true"
  [0]=>
  unicode(5) "false"
  [u"false"]=>
  bool(false)
  [u"true"]=>
  bool(true)
}
-- Iteration 15 --
array(3) {
  [u""]=>
  unicode(6) "emptys"
  [u"emptyd"]=>
  unicode(0) ""
  [u"emptys"]=>
  unicode(0) ""
}
-- Iteration 16 --
array(6) {
  [1]=>
  unicode(0) ""
  [2]=>
  unicode(0) ""
  [3]=>
  NULL
  [4]=>
  NULL
  [5]=>
  bool(false)
  [6]=>
  bool(true)
}
-- Iteration 17 --
array(3) {
  [u""]=>
  int(4)
  [0]=>
  int(5)
  [1]=>
  int(6)
}
-- Iteration 18 --
array(3) {
  [u"One"]=>
  int(10)
  [u"two"]=>
  int(20)
  [u"three"]=>
  int(3)
}
Done
