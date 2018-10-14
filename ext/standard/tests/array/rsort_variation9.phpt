--TEST--
Test rsort() function : usage variations - mixed associative arrays
--FILE--
<?php
/* Prototype  : bool rsort(array &$array_arg [, int $sort_flags])
 * Description: Sort an array in reverse order
 * Source code: ext/standard/array.c
 */

/*
 * Pass rsort() associative arrays to test key re-assignment
 */

echo "*** Testing rsort() : variation ***\n";

// Associative arrays
$various_arrays = array(
	// numeric assoc. only array
	array(5 => 55, 6 => 66, 2 => 22, 3 => 33, 1 => 11),

	// two-dimensional assoc. and default key array
	array("fruits"  => array("a" => "orange", "b" => "banana", "c" => "apple"),
     	  "numbers" => array(1, 2, 3, 4, 5, 6),
     	  "holes"   => array("first", 5 => "second", "third")),

	// numeric assoc. and default key array
	array(1, 1, 8 => 1,  4 => 1, 19, 3 => 13),

	// mixed assoc. array
	array('bar' => 'baz', "foo" => 1),

	// assoc. only multi-dimensional array
	array('a' => 1,'b' => array('e' => 2,'f' => 3),'c' => array('g' => 4),'d' => 5),
);

$count = 1;

// loop through to test rsort() with different arrays,
// to test the new keys for the elements in the sorted array
foreach ($various_arrays as $array) {
  echo "\n-- Iteration $count --\n";

  echo "-- Sort flag = default --\n";
  $temp_array = $array;
  var_dump(rsort($temp_array) );
  var_dump($temp_array);

  echo "-- Sort flag = SORT_REGULAR --\n";
  $temp_array = $array;
  var_dump(rsort($temp_array, SORT_REGULAR) );
  var_dump($temp_array);
  $count++;
}

echo "Done";
?>
--EXPECT--
*** Testing rsort() : variation ***

-- Iteration 1 --
-- Sort flag = default --
bool(true)
array(5) {
  [0]=>
  int(66)
  [1]=>
  int(55)
  [2]=>
  int(33)
  [3]=>
  int(22)
  [4]=>
  int(11)
}
-- Sort flag = SORT_REGULAR --
bool(true)
array(5) {
  [0]=>
  int(66)
  [1]=>
  int(55)
  [2]=>
  int(33)
  [3]=>
  int(22)
  [4]=>
  int(11)
}

-- Iteration 2 --
-- Sort flag = default --
bool(true)
array(3) {
  [0]=>
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
  [1]=>
  array(3) {
    ["a"]=>
    string(6) "orange"
    ["b"]=>
    string(6) "banana"
    ["c"]=>
    string(5) "apple"
  }
  [2]=>
  array(3) {
    [0]=>
    string(5) "first"
    [5]=>
    string(6) "second"
    [6]=>
    string(5) "third"
  }
}
-- Sort flag = SORT_REGULAR --
bool(true)
array(3) {
  [0]=>
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
  [1]=>
  array(3) {
    ["a"]=>
    string(6) "orange"
    ["b"]=>
    string(6) "banana"
    ["c"]=>
    string(5) "apple"
  }
  [2]=>
  array(3) {
    [0]=>
    string(5) "first"
    [5]=>
    string(6) "second"
    [6]=>
    string(5) "third"
  }
}

-- Iteration 3 --
-- Sort flag = default --
bool(true)
array(6) {
  [0]=>
  int(19)
  [1]=>
  int(13)
  [2]=>
  int(1)
  [3]=>
  int(1)
  [4]=>
  int(1)
  [5]=>
  int(1)
}
-- Sort flag = SORT_REGULAR --
bool(true)
array(6) {
  [0]=>
  int(19)
  [1]=>
  int(13)
  [2]=>
  int(1)
  [3]=>
  int(1)
  [4]=>
  int(1)
  [5]=>
  int(1)
}

-- Iteration 4 --
-- Sort flag = default --
bool(true)
array(2) {
  [0]=>
  int(1)
  [1]=>
  string(3) "baz"
}
-- Sort flag = SORT_REGULAR --
bool(true)
array(2) {
  [0]=>
  int(1)
  [1]=>
  string(3) "baz"
}

-- Iteration 5 --
-- Sort flag = default --
bool(true)
array(4) {
  [0]=>
  array(2) {
    ["e"]=>
    int(2)
    ["f"]=>
    int(3)
  }
  [1]=>
  array(1) {
    ["g"]=>
    int(4)
  }
  [2]=>
  int(5)
  [3]=>
  int(1)
}
-- Sort flag = SORT_REGULAR --
bool(true)
array(4) {
  [0]=>
  array(2) {
    ["e"]=>
    int(2)
    ["f"]=>
    int(3)
  }
  [1]=>
  array(1) {
    ["g"]=>
    int(4)
  }
  [2]=>
  int(5)
  [3]=>
  int(1)
}
Done
