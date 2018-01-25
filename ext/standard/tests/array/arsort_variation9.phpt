--TEST--
Test arsort() function : usage variations - sorting arrays with/without keys, 'sort_flags' as default/SORT_REGULAR
--FILE--
<?php
/* Prototype  : bool arsort ( array &$array [, int $sort_flags] )
 * Description: Sort an array and maintain index association.
                Elements will be arranged from highest to lowest when this function has completed.
 * Source code: ext/standard/array.c
*/

/*
 * Testing arsort() by providing arrays  with key values for $array argument with following flag values.
 * 1.flag value as default
 * 2.SORT_REGULAR - compare items normally
 */

echo "*** Testing arsort() : usage variations ***\n";

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
echo "\n-- Testing arsort() by supplying various arrays with key values --\n";

// loop through to test arsort() with different arrays,
// to test the new keys for the elements in the sorted array
foreach ($various_arrays as $array) {
  echo "\n-- Iteration $count --\n";

  echo "- With default sort_flag -\n";
  $temp_array = $array;
  var_dump(arsort($temp_array) );
  var_dump($temp_array);

  echo "- Sort_flag = SORT_REGULAR -\n";
  $temp_array = $array;
  var_dump(arsort($temp_array, SORT_REGULAR) );
  var_dump($temp_array);
  $count++;
}

echo "Done\n";
?>
--EXPECTF--
*** Testing arsort() : usage variations ***

-- Testing arsort() by supplying various arrays with key values --

-- Iteration 1 --
- With default sort_flag -
bool(true)
array(5) {
  [6]=>
  int(66)
  [5]=>
  int(55)
  [8]=>
  int(33)
  [7]=>
  int(22)
  [9]=>
  int(11)
}
- Sort_flag = SORT_REGULAR -
bool(true)
array(5) {
  [6]=>
  int(66)
  [5]=>
  int(55)
  [8]=>
  int(33)
  [7]=>
  int(22)
  [9]=>
  int(11)
}

-- Iteration 2 --
- With default sort_flag -
bool(true)
array(3) {
  ["a"]=>
  string(6) "orange"
  [0]=>
  string(6) "banana"
  ["c"]=>
  string(5) "apple"
}
- Sort_flag = SORT_REGULAR -
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
- With default sort_flag -
bool(true)
array(6) {
  [5]=>
  int(6)
  [4]=>
  int(5)
  [3]=>
  int(4)
  [2]=>
  int(3)
  [1]=>
  int(2)
  [0]=>
  int(1)
}
- Sort_flag = SORT_REGULAR -
bool(true)
array(6) {
  [5]=>
  int(6)
  [4]=>
  int(5)
  [3]=>
  int(4)
  [2]=>
  int(3)
  [1]=>
  int(2)
  [0]=>
  int(1)
}

-- Iteration 4 --
- With default sort_flag -
bool(true)
array(3) {
  [6]=>
  string(5) "third"
  [5]=>
  string(6) "second"
  [0]=>
  string(5) "first"
}
- Sort_flag = SORT_REGULAR -
bool(true)
array(3) {
  [6]=>
  string(5) "third"
  [5]=>
  string(6) "second"
  [0]=>
  string(5) "first"
}

-- Iteration 5 --
- With default sort_flag -
bool(true)
array(6) {
  [9]=>
  int(19)
  [3]=>
  int(13)
  [0]=>
  int(1)
  [1]=>
  int(1)
  [8]=>
  int(1)
  [4]=>
  int(1)
}
- Sort_flag = SORT_REGULAR -
bool(true)
array(6) {
  [9]=>
  int(19)
  [3]=>
  int(13)
  [0]=>
  int(1)
  [1]=>
  int(1)
  [8]=>
  int(1)
  [4]=>
  int(1)
}

-- Iteration 6 --
- With default sort_flag -
bool(true)
array(2) {
  ["foo"]=>
  int(1)
  ["bar"]=>
  string(3) "baz"
}
- Sort_flag = SORT_REGULAR -
bool(true)
array(2) {
  ["foo"]=>
  int(1)
  ["bar"]=>
  string(3) "baz"
}

-- Iteration 7 --
- With default sort_flag -
bool(true)
array(4) {
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
  ["a"]=>
  int(1)
}
- Sort_flag = SORT_REGULAR -
bool(true)
array(4) {
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
  ["a"]=>
  int(1)
}
Done
