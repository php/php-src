--TEST--
Test array_values() function
--INI--
precision=14
--FILE--
<?php
echo "*** Testing array_values() on basic array ***\n";
$basic_arr = array( 1, 2, 2.0, "asdasd", array(1,2,3) );
var_dump( array_values($basic_arr) );

echo "\n*** Testing array_values() on various arrays ***";
$arrays = array (
  array(),
  array(0),
  array(-1),
  array( array() ),
  array("Hello"),
  array(""),
  array("", array()),
  array(1,2,3),
  array(1,2,3, array()),
  array(1,2,3, array(4,6)),
  array("a" => 1, "b" => 2, "c" =>3),
  array(0 => 0, 1 => 1, 2 => 2),
  array(TRUE, FALSE, NULL, true, false, null, "TRUE", "FALSE",
        "NULL", "\x000", "\000"),
  array("Hi" => 1, "Hello" => 2, "World" => 3),
  array("a" => "abcd", "a" => "", "ab" => -6, "cd" => -0.5 ),
  array(0 => array(), 1=> array(0), 2 => array(1), ""=> array(), ""=>"" )
);

$i = 0;
/* loop through to test array_values() with different arrays given above */
foreach ($arrays as $array) {
  echo "\n-- Iteration $i --\n";
  var_dump( array_values($array) );
  $i++;
}

echo "Done\n";
?>
--EXPECTF--
*** Testing array_values() on basic array ***
array(5) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  float(2)
  [3]=>
  string(6) "asdasd"
  [4]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
}

*** Testing array_values() on various arrays ***
-- Iteration 0 --
array(0) {
}

-- Iteration 1 --
array(1) {
  [0]=>
  int(0)
}

-- Iteration 2 --
array(1) {
  [0]=>
  int(-1)
}

-- Iteration 3 --
array(1) {
  [0]=>
  array(0) {
  }
}

-- Iteration 4 --
array(1) {
  [0]=>
  string(5) "Hello"
}

-- Iteration 5 --
array(1) {
  [0]=>
  string(0) ""
}

-- Iteration 6 --
array(2) {
  [0]=>
  string(0) ""
  [1]=>
  array(0) {
  }
}

-- Iteration 7 --
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}

-- Iteration 8 --
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  array(0) {
  }
}

-- Iteration 9 --
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  array(2) {
    [0]=>
    int(4)
    [1]=>
    int(6)
  }
}

-- Iteration 10 --
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}

-- Iteration 11 --
array(3) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
}

-- Iteration 12 --
array(11) {
  [0]=>
  bool(true)
  [1]=>
  bool(false)
  [2]=>
  NULL
  [3]=>
  bool(true)
  [4]=>
  bool(false)
  [5]=>
  NULL
  [6]=>
  string(4) "TRUE"
  [7]=>
  string(5) "FALSE"
  [8]=>
  string(4) "NULL"
  [9]=>
  string(2) "%00"
  [10]=>
  string(1) "%0"
}

-- Iteration 13 --
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}

-- Iteration 14 --
array(3) {
  [0]=>
  string(0) ""
  [1]=>
  int(-6)
  [2]=>
  float(-0.5)
}

-- Iteration 15 --
array(4) {
  [0]=>
  array(0) {
  }
  [1]=>
  array(1) {
    [0]=>
    int(0)
  }
  [2]=>
  array(1) {
    [0]=>
    int(1)
  }
  [3]=>
  string(0) ""
}
Done
