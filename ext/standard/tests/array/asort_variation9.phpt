--TEST--
Test asort() function : usage variations - sorting arrays with/without keys, 'sort_flags' as default/SORT_REGULAR
--FILE--
<?php
/* Prototype  : bool asort ( array &$array [, int $sort_flags] )
 * Description: Sort an array and maintain index association.
                Elements will be arranged from lowest to highest when this function has completed.
 * Source code: ext/standard/array.c
*/

/*
 * Testing asort() by providing arrays  with key values for $array argument with following flag values.
 * 1.flag value as default
 * 2.SORT_REGULAR - compare items normally
 */

echo "*** Testing asort() : usage variations ***\n";

// list of arrays with/without key values
$various_arrays = array (
  array(5 => 55,  66, 22, 33, 11),
  array ("a" => "orange",  "banana", "c" => "apple"),
  array(1, 2, 3, 4, 5, 6),
  array("first", 5 => "second", "third"),
  array(1, 1, 8 => 1,  4 => 1, 19, 3 => 13),
  array('bar' => 'baz', "foo" => 1),
  array('a'=>1,'b'=>array('e'=>2,'f'=>3),'c'=>array('g'=>4),'d'=>5),
);

$count = 1;
echo "\n-- Testing asort() by supplying various arrays with key values --\n";

// loop through to test asort() with different arrays,
// to test the new keys for the elements in the sorted array
foreach ($various_arrays as $array) {
  echo "\n-- Iteration $count --\n";

  echo "- With default sort_flag -\n";
  $temp_array = $array;
  var_dump(asort($temp_array) );
  var_dump($temp_array);

  echo "- Sort_flag = SORT_REGULAR -\n";
  $temp_array = $array;
  var_dump(asort($temp_array, SORT_REGULAR) );
  var_dump($temp_array);
  $count++;
}

echo "Done\n";
?>
--EXPECTF--
*** Testing asort() : usage variations ***

-- Testing asort() by supplying various arrays with key values --

-- Iteration 1 --
- With default sort_flag -
bool(true)
array(5) {
  [9]=>
  int(11)
  [7]=>
  int(22)
  [8]=>
  int(33)
  [5]=>
  int(55)
  [6]=>
  int(66)
}
- Sort_flag = SORT_REGULAR -
bool(true)
array(5) {
  [9]=>
  int(11)
  [7]=>
  int(22)
  [8]=>
  int(33)
  [5]=>
  int(55)
  [6]=>
  int(66)
}

-- Iteration 2 --
- With default sort_flag -
bool(true)
array(3) {
  ["c"]=>
  string(5) "apple"
  [0]=>
  string(6) "banana"
  ["a"]=>
  string(6) "orange"
}
- Sort_flag = SORT_REGULAR -
bool(true)
array(3) {
  ["c"]=>
  string(5) "apple"
  [0]=>
  string(6) "banana"
  ["a"]=>
  string(6) "orange"
}

-- Iteration 3 --
- With default sort_flag -
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
- Sort_flag = SORT_REGULAR -
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
- With default sort_flag -
bool(true)
array(3) {
  [0]=>
  string(5) "first"
  [5]=>
  string(6) "second"
  [6]=>
  string(5) "third"
}
- Sort_flag = SORT_REGULAR -
bool(true)
array(3) {
  [0]=>
  string(5) "first"
  [5]=>
  string(6) "second"
  [6]=>
  string(5) "third"
}

-- Iteration 5 --
- With default sort_flag -
bool(true)
array(6) {
  [0]=>
  int(1)
  [1]=>
  int(1)
  [8]=>
  int(1)
  [4]=>
  int(1)
  [3]=>
  int(13)
  [9]=>
  int(19)
}
- Sort_flag = SORT_REGULAR -
bool(true)
array(6) {
  [0]=>
  int(1)
  [1]=>
  int(1)
  [8]=>
  int(1)
  [4]=>
  int(1)
  [3]=>
  int(13)
  [9]=>
  int(19)
}

-- Iteration 6 --
- With default sort_flag -
bool(true)
array(2) {
  ["bar"]=>
  string(3) "baz"
  ["foo"]=>
  int(1)
}
- Sort_flag = SORT_REGULAR -
bool(true)
array(2) {
  ["bar"]=>
  string(3) "baz"
  ["foo"]=>
  int(1)
}

-- Iteration 7 --
- With default sort_flag -
bool(true)
array(4) {
  ["a"]=>
  int(1)
  ["d"]=>
  int(5)
  ["c"]=>
  array(1) {
    ["g"]=>
    int(4)
  }
  ["b"]=>
  array(2) {
    ["e"]=>
    int(2)
    ["f"]=>
    int(3)
  }
}
- Sort_flag = SORT_REGULAR -
bool(true)
array(4) {
  ["a"]=>
  int(1)
  ["d"]=>
  int(5)
  ["c"]=>
  array(1) {
    ["g"]=>
    int(4)
  }
  ["b"]=>
  array(2) {
    ["e"]=>
    int(2)
    ["f"]=>
    int(3)
  }
}
Done
