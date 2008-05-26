--TEST--
Test array_reverse() function : usage variations - different array values for 'array' argument
--FILE--
<?php
/* Prototype  : array array_reverse(array $array [, bool $preserve_keys])
 * Description: Return input as a new array with the order of the entries reversed
 * Source code: ext/standard/array.c
*/

/*
 * Testing the functionality of array_reverse() by giving
 * different array values for $array argument
*/

echo "*** Testing array_reverse() : usage variations ***\n";

//get an unset variable
$unset_var = 10;
unset ($unset_var);

//get a resource variable
$fp = fopen(__FILE__, "r");

//get a class
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

// loop through the various elements of $arrays to test array_reverse()
$iterator = 1;
foreach($arrays as $array) {
  echo "-- Iteration $iterator --\n";
  // with default argument
  echo "- with default argument -\n";
  var_dump( array_reverse($array) );
  // with all possible arguments
  echo "- with \$preserve keys = true -\n";
  var_dump( array_reverse($array, true) );
  echo "- with \$preserve_keys = false -\n";
  var_dump( array_reverse($array, false) );
  $iterator++;
};

// close the file resource used
fclose($fp);

echo "Done";
?>
--EXPECT--
*** Testing array_reverse() : usage variations ***
-- Iteration 1 --
- with default argument -
array(2) {
  [0]=>
  int(2)
  [1]=>
  int(1)
}
- with $preserve keys = true -
array(2) {
  [1]=>
  int(2)
  [0]=>
  int(1)
}
- with $preserve_keys = false -
array(2) {
  [0]=>
  int(2)
  [1]=>
  int(1)
}
-- Iteration 2 --
- with default argument -
array(2) {
  [0]=>
  float(2.2)
  [1]=>
  float(1.1)
}
- with $preserve keys = true -
array(2) {
  [1]=>
  float(2.2)
  [0]=>
  float(1.1)
}
- with $preserve_keys = false -
array(2) {
  [0]=>
  float(2.2)
  [1]=>
  float(1.1)
}
-- Iteration 3 --
- with default argument -
array(2) {
  [0]=>
  array(1) {
    [0]=>
    int(1)
  }
  [1]=>
  array(1) {
    [0]=>
    int(2)
  }
}
- with $preserve keys = true -
array(2) {
  [1]=>
  array(1) {
    [0]=>
    int(1)
  }
  [0]=>
  array(1) {
    [0]=>
    int(2)
  }
}
- with $preserve_keys = false -
array(2) {
  [0]=>
  array(1) {
    [0]=>
    int(1)
  }
  [1]=>
  array(1) {
    [0]=>
    int(2)
  }
}
-- Iteration 4 --
- with default argument -
array(2) {
  [0]=>
  bool(true)
  [1]=>
  bool(false)
}
- with $preserve keys = true -
array(2) {
  [1]=>
  bool(true)
  [0]=>
  bool(false)
}
- with $preserve_keys = false -
array(2) {
  [0]=>
  bool(true)
  [1]=>
  bool(false)
}
-- Iteration 5 --
- with default argument -
array(0) {
}
- with $preserve keys = true -
array(0) {
}
- with $preserve_keys = false -
array(0) {
}
-- Iteration 6 --
- with default argument -
array(1) {
  [0]=>
  NULL
}
- with $preserve keys = true -
array(1) {
  [0]=>
  NULL
}
- with $preserve_keys = false -
array(1) {
  [0]=>
  NULL
}
-- Iteration 7 --
- with default argument -
array(6) {
  [0]=>
  unicode(5) "ccccc"
  [1]=>
  unicode(1) "c"
  [2]=>
  unicode(4) "bbbb"
  [3]=>
  unicode(1) "b"
  [4]=>
  unicode(4) "aaaa"
  [5]=>
  unicode(1) "a"
}
- with $preserve keys = true -
array(6) {
  [5]=>
  unicode(5) "ccccc"
  [4]=>
  unicode(1) "c"
  [3]=>
  unicode(4) "bbbb"
  [2]=>
  unicode(1) "b"
  [1]=>
  unicode(4) "aaaa"
  [0]=>
  unicode(1) "a"
}
- with $preserve_keys = false -
array(6) {
  [0]=>
  unicode(5) "ccccc"
  [1]=>
  unicode(1) "c"
  [2]=>
  unicode(4) "bbbb"
  [3]=>
  unicode(1) "b"
  [4]=>
  unicode(4) "aaaa"
  [5]=>
  unicode(1) "a"
}
-- Iteration 8 --
- with default argument -
array(3) {
  [0]=>
  unicode(5) "three"
  [1]=>
  unicode(3) "two"
  [2]=>
  unicode(3) "one"
}
- with $preserve keys = true -
array(3) {
  [3]=>
  unicode(5) "three"
  [2]=>
  unicode(3) "two"
  [1]=>
  unicode(3) "one"
}
- with $preserve_keys = false -
array(3) {
  [0]=>
  unicode(5) "three"
  [1]=>
  unicode(3) "two"
  [2]=>
  unicode(3) "one"
}
-- Iteration 9 --
- with default argument -
array(3) {
  [u"three"]=>
  int(3)
  [u"two"]=>
  int(2)
  [u"one"]=>
  int(1)
}
- with $preserve keys = true -
array(3) {
  [u"three"]=>
  int(3)
  [u"two"]=>
  int(2)
  [u"one"]=>
  int(1)
}
- with $preserve_keys = false -
array(3) {
  [u"three"]=>
  int(3)
  [u"two"]=>
  int(2)
  [u"one"]=>
  int(1)
}
-- Iteration 10 --
- with default argument -
array(4) {
  [0]=>
  int(30)
  [1]=>
  int(40)
  [2]=>
  int(20)
  [3]=>
  int(10)
}
- with $preserve keys = true -
array(4) {
  [3]=>
  int(30)
  [4]=>
  int(40)
  [2]=>
  int(20)
  [1]=>
  int(10)
}
- with $preserve_keys = false -
array(4) {
  [0]=>
  int(30)
  [1]=>
  int(40)
  [2]=>
  int(20)
  [3]=>
  int(10)
}
-- Iteration 11 --
- with default argument -
array(3) {
  [u"three"]=>
  unicode(6) "thirty"
  [u"two"]=>
  unicode(6) "twenty"
  [u"one"]=>
  unicode(3) "ten"
}
- with $preserve keys = true -
array(3) {
  [u"three"]=>
  unicode(6) "thirty"
  [u"two"]=>
  unicode(6) "twenty"
  [u"one"]=>
  unicode(3) "ten"
}
- with $preserve_keys = false -
array(3) {
  [u"three"]=>
  unicode(6) "thirty"
  [u"two"]=>
  unicode(6) "twenty"
  [u"one"]=>
  unicode(3) "ten"
}
-- Iteration 12 --
- with default argument -
array(3) {
  [0]=>
  unicode(4) "four"
  [1]=>
  unicode(3) "two"
  [u"one"]=>
  int(1)
}
- with $preserve keys = true -
array(3) {
  [4]=>
  unicode(4) "four"
  [2]=>
  unicode(3) "two"
  [u"one"]=>
  int(1)
}
- with $preserve_keys = false -
array(3) {
  [0]=>
  unicode(4) "four"
  [1]=>
  unicode(3) "two"
  [u"one"]=>
  int(1)
}
-- Iteration 13 --
- with default argument -
array(3) {
  [u"null"]=>
  NULL
  [u"NULL"]=>
  NULL
  [u""]=>
  unicode(4) "null"
}
- with $preserve keys = true -
array(3) {
  [u"null"]=>
  NULL
  [u"NULL"]=>
  NULL
  [u""]=>
  unicode(4) "null"
}
- with $preserve_keys = false -
array(3) {
  [u"null"]=>
  NULL
  [u"NULL"]=>
  NULL
  [u""]=>
  unicode(4) "null"
}
-- Iteration 14 --
- with default argument -
array(4) {
  [u"true"]=>
  bool(true)
  [u"false"]=>
  bool(false)
  [0]=>
  unicode(5) "false"
  [1]=>
  unicode(4) "true"
}
- with $preserve keys = true -
array(4) {
  [u"true"]=>
  bool(true)
  [u"false"]=>
  bool(false)
  [0]=>
  unicode(5) "false"
  [1]=>
  unicode(4) "true"
}
- with $preserve_keys = false -
array(4) {
  [u"true"]=>
  bool(true)
  [u"false"]=>
  bool(false)
  [0]=>
  unicode(5) "false"
  [1]=>
  unicode(4) "true"
}
-- Iteration 15 --
- with default argument -
array(3) {
  [u"emptys"]=>
  unicode(0) ""
  [u"emptyd"]=>
  unicode(0) ""
  [u""]=>
  unicode(6) "emptys"
}
- with $preserve keys = true -
array(3) {
  [u"emptys"]=>
  unicode(0) ""
  [u"emptyd"]=>
  unicode(0) ""
  [u""]=>
  unicode(6) "emptys"
}
- with $preserve_keys = false -
array(3) {
  [u"emptys"]=>
  unicode(0) ""
  [u"emptyd"]=>
  unicode(0) ""
  [u""]=>
  unicode(6) "emptys"
}
-- Iteration 16 --
- with default argument -
array(6) {
  [0]=>
  bool(true)
  [1]=>
  bool(false)
  [2]=>
  NULL
  [3]=>
  NULL
  [4]=>
  unicode(0) ""
  [5]=>
  unicode(0) ""
}
- with $preserve keys = true -
array(6) {
  [6]=>
  bool(true)
  [5]=>
  bool(false)
  [4]=>
  NULL
  [3]=>
  NULL
  [2]=>
  unicode(0) ""
  [1]=>
  unicode(0) ""
}
- with $preserve_keys = false -
array(6) {
  [0]=>
  bool(true)
  [1]=>
  bool(false)
  [2]=>
  NULL
  [3]=>
  NULL
  [4]=>
  unicode(0) ""
  [5]=>
  unicode(0) ""
}
-- Iteration 17 --
- with default argument -
array(3) {
  [0]=>
  int(6)
  [1]=>
  int(5)
  [u""]=>
  int(4)
}
- with $preserve keys = true -
array(3) {
  [1]=>
  int(6)
  [0]=>
  int(5)
  [u""]=>
  int(4)
}
- with $preserve_keys = false -
array(3) {
  [0]=>
  int(6)
  [1]=>
  int(5)
  [u""]=>
  int(4)
}
-- Iteration 18 --
- with default argument -
array(3) {
  [u"three"]=>
  int(3)
  [u"two"]=>
  int(20)
  [u"One"]=>
  int(10)
}
- with $preserve keys = true -
array(3) {
  [u"three"]=>
  int(3)
  [u"two"]=>
  int(20)
  [u"One"]=>
  int(10)
}
- with $preserve_keys = false -
array(3) {
  [u"three"]=>
  int(3)
  [u"two"]=>
  int(20)
  [u"One"]=>
  int(10)
}
Done
