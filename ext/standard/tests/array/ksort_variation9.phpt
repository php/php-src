--TEST--
Test ksort() function : usage variations - sorting arrays with/without keys
--FILE--
<?php
/*
 * Testing ksort() by providing arrays  with/without key values for $array argument with following flag values:
 *  1.flag value as default
 *  2.SORT_REGULAR - compare items normally
 */

echo "*** Testing ksort() : usage variations ***\n";

// list of arrays with/without key values
$various_arrays = array (
  array(5 => 55,  66, 22, 33, 11),
  array ("a" => "orange",  "banana", "c" => "apple"),
  array(1, 2, 3, 4, 5, 6),
  array("first", 5 => "second", 1 => "third"),
  array(1, 1, 8 => 1,  4 => 1, 19, 3 => 13),
  array('bar' => 'baz', "foo" => 1),
  array('a' => 1,'b' => array('e' => 2,'f' => 3),'c' => array('g' => 4),'d' => 5),
);

$count = 1;
echo "\n-- Testing ksort() by supplying various arrays with/without key values --\n";

// loop through to test ksort() with different arrays,
foreach ($various_arrays as $array) {
  echo "\n-- Iteration $count --\n";

  echo "- With default sort flag -\n";
  $temp_array = $array;
  var_dump( ksort($temp_array) );
  var_dump($temp_array);

  echo "- Sort flag = SORT_REGULAR -\n";
  $temp_array = $array;
  var_dump( ksort($temp_array, SORT_REGULAR) );
  var_dump($temp_array);
  $count++;
}

echo "Done\n";
?>
--EXPECT--
*** Testing ksort() : usage variations ***

-- Testing ksort() by supplying various arrays with/without key values --

-- Iteration 1 --
- With default sort flag -
bool(true)
array(5) {
  [5]=>
  int(55)
  [6]=>
  int(66)
  [7]=>
  int(22)
  [8]=>
  int(33)
  [9]=>
  int(11)
}
- Sort flag = SORT_REGULAR -
bool(true)
array(5) {
  [5]=>
  int(55)
  [6]=>
  int(66)
  [7]=>
  int(22)
  [8]=>
  int(33)
  [9]=>
  int(11)
}

-- Iteration 2 --
- With default sort flag -
bool(true)
array(3) {
  ["a"]=>
  string(6) "orange"
  [0]=>
  string(6) "banana"
  ["c"]=>
  string(5) "apple"
}
- Sort flag = SORT_REGULAR -
bool(true)
array(3) {
  ["a"]=>
  string(6) "orange"
  [0]=>
  string(6) "banana"
  ["c"]=>
  string(5) "apple"
}

-- Iteration 3 --
- With default sort flag -
bool(true)
array(6) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
  [4]=>
  int(5)
  [5]=>
  int(6)
}
- Sort flag = SORT_REGULAR -
bool(true)
array(6) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
  [4]=>
  int(5)
  [5]=>
  int(6)
}

-- Iteration 4 --
- With default sort flag -
bool(true)
array(3) {
  [0]=>
  string(5) "first"
  [1]=>
  string(5) "third"
  [5]=>
  string(6) "second"
}
- Sort flag = SORT_REGULAR -
bool(true)
array(3) {
  [0]=>
  string(5) "first"
  [1]=>
  string(5) "third"
  [5]=>
  string(6) "second"
}

-- Iteration 5 --
- With default sort flag -
bool(true)
array(6) {
  [0]=>
  int(1)
  [1]=>
  int(1)
  [3]=>
  int(13)
  [4]=>
  int(1)
  [8]=>
  int(1)
  [9]=>
  int(19)
}
- Sort flag = SORT_REGULAR -
bool(true)
array(6) {
  [0]=>
  int(1)
  [1]=>
  int(1)
  [3]=>
  int(13)
  [4]=>
  int(1)
  [8]=>
  int(1)
  [9]=>
  int(19)
}

-- Iteration 6 --
- With default sort flag -
bool(true)
array(2) {
  ["bar"]=>
  string(3) "baz"
  ["foo"]=>
  int(1)
}
- Sort flag = SORT_REGULAR -
bool(true)
array(2) {
  ["bar"]=>
  string(3) "baz"
  ["foo"]=>
  int(1)
}

-- Iteration 7 --
- With default sort flag -
bool(true)
array(4) {
  ["a"]=>
  int(1)
  ["b"]=>
  array(2) {
    ["e"]=>
    int(2)
    ["f"]=>
    int(3)
  }
  ["c"]=>
  array(1) {
    ["g"]=>
    int(4)
  }
  ["d"]=>
  int(5)
}
- Sort flag = SORT_REGULAR -
bool(true)
array(4) {
  ["a"]=>
  int(1)
  ["b"]=>
  array(2) {
    ["e"]=>
    int(2)
    ["f"]=>
    int(3)
  }
  ["c"]=>
  array(1) {
    ["g"]=>
    int(4)
  }
  ["d"]=>
  int(5)
}
Done
