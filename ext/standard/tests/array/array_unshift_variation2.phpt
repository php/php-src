--TEST--
Test array_unshift() function : usage variations - all possible values for 'var' argument
--FILE--
<?php
/* Prototype  : int array_unshift(array $array, mixed $var [, mixed ...])
 * Description: Pushes elements onto the beginning of the array 
 * Source code: ext/standard/array.c
*/

/*
 * Testing array_unshift() by giving all the possible values for $var argument
*/

echo "*** Testing array_unshift() : all possible values for \$var argument ***\n";

// array to be passed to $array argument
$array = array('f' => "first", "s" => 'second', 1, 2.222);

// get a class 
class classA
{
  public function __toString() {
    return "Class A object";
  }
}

// get a resource variable
$fp = fopen(__FILE__, "r");

// heredoc string
$heredoc = <<<EOT
hello world
EOT;

// get an unset variable
$unset_var = 10;
unset ($unset_var);

// different types of values to be passed to $var argument
$vars = array(

       // int data
/*1*/  0,
       1,
       12345,
       -2345,
 
       // float data
/*5*/  10.5,
       -10.5,
       12.3456789000e10,
       12.3456789000E-10,
       .5,

       // array data
/*10*/ array(),
       array(0),
       array(1),
       array(1, 2),
       array('color' => 'red', 'item' => 'pen'),
 
       // null data
/*15*/ NULL,
       null,

       // boolean data
/*17*/ true,
       false,
       TRUE,
       FALSE,
 
       // empty data
/*21*/ "",
       '',

       // string data
/*23*/ "string",
       'string',
       $heredoc,

       // object data
/*26*/ new classA(),

       // undefined data
       @$undefined_var,

       // unset data
       @$unset_var,
 
       // resource variable
/*29*/ $fp
);

// loop through each element of $vars to check the functionality of array_unshift()
$iterator = 1;
foreach($vars as $var) {
  echo "-- Iteration $iterator --\n";
  $temp_array = $array;

  /* with default argument */
  // returns element count in the resulting array after arguments are pushed to
  // beginning of the given array
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

// close the file resource used
fclose($fp);

echo "Done";
?>
--EXPECTF--
*** Testing array_unshift() : all possible values for $var argument ***
-- Iteration 1 --
int(5)
array(5) {
  [0]=>
  int(0)
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [1]=>
  int(1)
  [2]=>
  float(2.222)
}
int(7)
array(7) {
  [0]=>
  int(0)
  [1]=>
  unicode(5) "hello"
  [2]=>
  unicode(5) "world"
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [3]=>
  int(1)
  [4]=>
  float(2.222)
}
-- Iteration 2 --
int(5)
array(5) {
  [0]=>
  int(1)
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [1]=>
  int(1)
  [2]=>
  float(2.222)
}
int(7)
array(7) {
  [0]=>
  int(1)
  [1]=>
  unicode(5) "hello"
  [2]=>
  unicode(5) "world"
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [3]=>
  int(1)
  [4]=>
  float(2.222)
}
-- Iteration 3 --
int(5)
array(5) {
  [0]=>
  int(12345)
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [1]=>
  int(1)
  [2]=>
  float(2.222)
}
int(7)
array(7) {
  [0]=>
  int(12345)
  [1]=>
  unicode(5) "hello"
  [2]=>
  unicode(5) "world"
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [3]=>
  int(1)
  [4]=>
  float(2.222)
}
-- Iteration 4 --
int(5)
array(5) {
  [0]=>
  int(-2345)
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [1]=>
  int(1)
  [2]=>
  float(2.222)
}
int(7)
array(7) {
  [0]=>
  int(-2345)
  [1]=>
  unicode(5) "hello"
  [2]=>
  unicode(5) "world"
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [3]=>
  int(1)
  [4]=>
  float(2.222)
}
-- Iteration 5 --
int(5)
array(5) {
  [0]=>
  float(10.5)
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [1]=>
  int(1)
  [2]=>
  float(2.222)
}
int(7)
array(7) {
  [0]=>
  float(10.5)
  [1]=>
  unicode(5) "hello"
  [2]=>
  unicode(5) "world"
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [3]=>
  int(1)
  [4]=>
  float(2.222)
}
-- Iteration 6 --
int(5)
array(5) {
  [0]=>
  float(-10.5)
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [1]=>
  int(1)
  [2]=>
  float(2.222)
}
int(7)
array(7) {
  [0]=>
  float(-10.5)
  [1]=>
  unicode(5) "hello"
  [2]=>
  unicode(5) "world"
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [3]=>
  int(1)
  [4]=>
  float(2.222)
}
-- Iteration 7 --
int(5)
array(5) {
  [0]=>
  float(123456789000)
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [1]=>
  int(1)
  [2]=>
  float(2.222)
}
int(7)
array(7) {
  [0]=>
  float(123456789000)
  [1]=>
  unicode(5) "hello"
  [2]=>
  unicode(5) "world"
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [3]=>
  int(1)
  [4]=>
  float(2.222)
}
-- Iteration 8 --
int(5)
array(5) {
  [0]=>
  float(1.23456789E-9)
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [1]=>
  int(1)
  [2]=>
  float(2.222)
}
int(7)
array(7) {
  [0]=>
  float(1.23456789E-9)
  [1]=>
  unicode(5) "hello"
  [2]=>
  unicode(5) "world"
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [3]=>
  int(1)
  [4]=>
  float(2.222)
}
-- Iteration 9 --
int(5)
array(5) {
  [0]=>
  float(0.5)
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [1]=>
  int(1)
  [2]=>
  float(2.222)
}
int(7)
array(7) {
  [0]=>
  float(0.5)
  [1]=>
  unicode(5) "hello"
  [2]=>
  unicode(5) "world"
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [3]=>
  int(1)
  [4]=>
  float(2.222)
}
-- Iteration 10 --
int(5)
array(5) {
  [0]=>
  array(0) {
  }
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [1]=>
  int(1)
  [2]=>
  float(2.222)
}
int(7)
array(7) {
  [0]=>
  array(0) {
  }
  [1]=>
  unicode(5) "hello"
  [2]=>
  unicode(5) "world"
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [3]=>
  int(1)
  [4]=>
  float(2.222)
}
-- Iteration 11 --
int(5)
array(5) {
  [0]=>
  array(1) {
    [0]=>
    int(0)
  }
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [1]=>
  int(1)
  [2]=>
  float(2.222)
}
int(7)
array(7) {
  [0]=>
  array(1) {
    [0]=>
    int(0)
  }
  [1]=>
  unicode(5) "hello"
  [2]=>
  unicode(5) "world"
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [3]=>
  int(1)
  [4]=>
  float(2.222)
}
-- Iteration 12 --
int(5)
array(5) {
  [0]=>
  array(1) {
    [0]=>
    int(1)
  }
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [1]=>
  int(1)
  [2]=>
  float(2.222)
}
int(7)
array(7) {
  [0]=>
  array(1) {
    [0]=>
    int(1)
  }
  [1]=>
  unicode(5) "hello"
  [2]=>
  unicode(5) "world"
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [3]=>
  int(1)
  [4]=>
  float(2.222)
}
-- Iteration 13 --
int(5)
array(5) {
  [0]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [1]=>
  int(1)
  [2]=>
  float(2.222)
}
int(7)
array(7) {
  [0]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
  [1]=>
  unicode(5) "hello"
  [2]=>
  unicode(5) "world"
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [3]=>
  int(1)
  [4]=>
  float(2.222)
}
-- Iteration 14 --
int(5)
array(5) {
  [0]=>
  array(2) {
    [u"color"]=>
    unicode(3) "red"
    [u"item"]=>
    unicode(3) "pen"
  }
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [1]=>
  int(1)
  [2]=>
  float(2.222)
}
int(7)
array(7) {
  [0]=>
  array(2) {
    [u"color"]=>
    unicode(3) "red"
    [u"item"]=>
    unicode(3) "pen"
  }
  [1]=>
  unicode(5) "hello"
  [2]=>
  unicode(5) "world"
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [3]=>
  int(1)
  [4]=>
  float(2.222)
}
-- Iteration 15 --
int(5)
array(5) {
  [0]=>
  NULL
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [1]=>
  int(1)
  [2]=>
  float(2.222)
}
int(7)
array(7) {
  [0]=>
  NULL
  [1]=>
  unicode(5) "hello"
  [2]=>
  unicode(5) "world"
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [3]=>
  int(1)
  [4]=>
  float(2.222)
}
-- Iteration 16 --
int(5)
array(5) {
  [0]=>
  NULL
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [1]=>
  int(1)
  [2]=>
  float(2.222)
}
int(7)
array(7) {
  [0]=>
  NULL
  [1]=>
  unicode(5) "hello"
  [2]=>
  unicode(5) "world"
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [3]=>
  int(1)
  [4]=>
  float(2.222)
}
-- Iteration 17 --
int(5)
array(5) {
  [0]=>
  bool(true)
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [1]=>
  int(1)
  [2]=>
  float(2.222)
}
int(7)
array(7) {
  [0]=>
  bool(true)
  [1]=>
  unicode(5) "hello"
  [2]=>
  unicode(5) "world"
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [3]=>
  int(1)
  [4]=>
  float(2.222)
}
-- Iteration 18 --
int(5)
array(5) {
  [0]=>
  bool(false)
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [1]=>
  int(1)
  [2]=>
  float(2.222)
}
int(7)
array(7) {
  [0]=>
  bool(false)
  [1]=>
  unicode(5) "hello"
  [2]=>
  unicode(5) "world"
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [3]=>
  int(1)
  [4]=>
  float(2.222)
}
-- Iteration 19 --
int(5)
array(5) {
  [0]=>
  bool(true)
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [1]=>
  int(1)
  [2]=>
  float(2.222)
}
int(7)
array(7) {
  [0]=>
  bool(true)
  [1]=>
  unicode(5) "hello"
  [2]=>
  unicode(5) "world"
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [3]=>
  int(1)
  [4]=>
  float(2.222)
}
-- Iteration 20 --
int(5)
array(5) {
  [0]=>
  bool(false)
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [1]=>
  int(1)
  [2]=>
  float(2.222)
}
int(7)
array(7) {
  [0]=>
  bool(false)
  [1]=>
  unicode(5) "hello"
  [2]=>
  unicode(5) "world"
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [3]=>
  int(1)
  [4]=>
  float(2.222)
}
-- Iteration 21 --
int(5)
array(5) {
  [0]=>
  unicode(0) ""
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [1]=>
  int(1)
  [2]=>
  float(2.222)
}
int(7)
array(7) {
  [0]=>
  unicode(0) ""
  [1]=>
  unicode(5) "hello"
  [2]=>
  unicode(5) "world"
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [3]=>
  int(1)
  [4]=>
  float(2.222)
}
-- Iteration 22 --
int(5)
array(5) {
  [0]=>
  unicode(0) ""
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [1]=>
  int(1)
  [2]=>
  float(2.222)
}
int(7)
array(7) {
  [0]=>
  unicode(0) ""
  [1]=>
  unicode(5) "hello"
  [2]=>
  unicode(5) "world"
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [3]=>
  int(1)
  [4]=>
  float(2.222)
}
-- Iteration 23 --
int(5)
array(5) {
  [0]=>
  unicode(6) "string"
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [1]=>
  int(1)
  [2]=>
  float(2.222)
}
int(7)
array(7) {
  [0]=>
  unicode(6) "string"
  [1]=>
  unicode(5) "hello"
  [2]=>
  unicode(5) "world"
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [3]=>
  int(1)
  [4]=>
  float(2.222)
}
-- Iteration 24 --
int(5)
array(5) {
  [0]=>
  unicode(6) "string"
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [1]=>
  int(1)
  [2]=>
  float(2.222)
}
int(7)
array(7) {
  [0]=>
  unicode(6) "string"
  [1]=>
  unicode(5) "hello"
  [2]=>
  unicode(5) "world"
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [3]=>
  int(1)
  [4]=>
  float(2.222)
}
-- Iteration 25 --
int(5)
array(5) {
  [0]=>
  unicode(11) "hello world"
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [1]=>
  int(1)
  [2]=>
  float(2.222)
}
int(7)
array(7) {
  [0]=>
  unicode(11) "hello world"
  [1]=>
  unicode(5) "hello"
  [2]=>
  unicode(5) "world"
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [3]=>
  int(1)
  [4]=>
  float(2.222)
}
-- Iteration 26 --
int(5)
array(5) {
  [0]=>
  object(classA)#%d (0) {
  }
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [1]=>
  int(1)
  [2]=>
  float(2.222)
}
int(7)
array(7) {
  [0]=>
  object(classA)#%d (0) {
  }
  [1]=>
  unicode(5) "hello"
  [2]=>
  unicode(5) "world"
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [3]=>
  int(1)
  [4]=>
  float(2.222)
}
-- Iteration 27 --
int(5)
array(5) {
  [0]=>
  NULL
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [1]=>
  int(1)
  [2]=>
  float(2.222)
}
int(7)
array(7) {
  [0]=>
  NULL
  [1]=>
  unicode(5) "hello"
  [2]=>
  unicode(5) "world"
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [3]=>
  int(1)
  [4]=>
  float(2.222)
}
-- Iteration 28 --
int(5)
array(5) {
  [0]=>
  NULL
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [1]=>
  int(1)
  [2]=>
  float(2.222)
}
int(7)
array(7) {
  [0]=>
  NULL
  [1]=>
  unicode(5) "hello"
  [2]=>
  unicode(5) "world"
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [3]=>
  int(1)
  [4]=>
  float(2.222)
}
-- Iteration 29 --
int(5)
array(5) {
  [0]=>
  resource(%d) of type (stream)
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [1]=>
  int(1)
  [2]=>
  float(2.222)
}
int(7)
array(7) {
  [0]=>
  resource(%d) of type (stream)
  [1]=>
  unicode(5) "hello"
  [2]=>
  unicode(5) "world"
  [u"f"]=>
  unicode(5) "first"
  [u"s"]=>
  unicode(6) "second"
  [3]=>
  int(1)
  [4]=>
  float(2.222)
}
Done
