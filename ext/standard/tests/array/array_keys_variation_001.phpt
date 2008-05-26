--TEST--
Test array_keys() function (variation - 1)
--FILE--
<?php

echo "\n*** Testing array_keys() on various arrays ***";
$arrays = array(
  array(), 
  array(0),
  array( array() ), 
  array("Hello" => "World"), 
  array("" => ""),  
  array(1,2,3, "d" => array(4,6, "d")),
  array("a" => 1, "b" => 2, "c" =>3, "d" => array()),
  array(0 => 0, 1 => 1, 2 => 2, 3 => 3),  
  array(0.001=>3.000, 1.002=>2, 1.999=>3, "a"=>3, 3=>5, "5"=>3.000),
  array(TRUE => TRUE, FALSE => FALSE, NULL => NULL, "\x000", "\000"),
  array("a" => "abcd", "a" => "", "ab" => -6, "cd" => -0.5 ),
  array(0 => array(), 1=> array(0), 2 => array(1), ""=> array(),""=>"" )
);

$i = 0;
/* loop through to test array_keys() with different arrays */
foreach ($arrays as $array) {
  echo "\n-- Iteration $i --\n";
  var_dump(array_keys($array)); 
  $i++;
}

echo "Done\n";
?>
--EXPECT--
*** Testing array_keys() on various arrays ***
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
  int(0)
}

-- Iteration 3 --
array(1) {
  [0]=>
  unicode(5) "Hello"
}

-- Iteration 4 --
array(1) {
  [0]=>
  unicode(0) ""
}

-- Iteration 5 --
array(4) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  unicode(1) "d"
}

-- Iteration 6 --
array(4) {
  [0]=>
  unicode(1) "a"
  [1]=>
  unicode(1) "b"
  [2]=>
  unicode(1) "c"
  [3]=>
  unicode(1) "d"
}

-- Iteration 7 --
array(4) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(3)
}

-- Iteration 8 --
array(5) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  unicode(1) "a"
  [3]=>
  int(3)
  [4]=>
  int(5)
}

-- Iteration 9 --
array(5) {
  [0]=>
  int(1)
  [1]=>
  int(0)
  [2]=>
  unicode(0) ""
  [3]=>
  int(2)
  [4]=>
  int(3)
}

-- Iteration 10 --
array(3) {
  [0]=>
  unicode(1) "a"
  [1]=>
  unicode(2) "ab"
  [2]=>
  unicode(2) "cd"
}

-- Iteration 11 --
array(4) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  unicode(0) ""
}
Done
