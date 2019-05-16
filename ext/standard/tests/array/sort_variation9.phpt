--TEST--
Test sort() function : usage variations - sort diff. associative arrays, 'sort_flags' as defualt/SORT_REGULAR
--FILE--
<?php
/* Prototype  : bool sort ( array &$array [, int $sort_flags] )
 * Description: This function sorts an array.
                Elements will be arranged from lowest to highest when this function has completed.
 * Source code: ext/standard/array.c
*/

/*
 * Testing sort() by providing arrays  with key values for $array argument
 * with following flag values.
 * 1.flag value as defualt
 * 2.SORT_REGULAR - compare items normally
 * To test the new keys for the elements in the sorted array.
 */

echo "*** Testing sort() : usage variations ***\n";

// list of arrays with key and values
$various_arrays = array(
  array(5 => 55, 6 => 66, 2 => 22, 3 => 33, 1 => 11),
  array ("fruits"  => array("a" => "orange", "b" => "banana", "c" => "apple"),
         "numbers" => array(1, 2, 3, 4, 5, 6),
         "holes"   => array("first", 5 => "second", "third")
        ),
  array(1, 1, 8 => 1,  4 => 1, 19, 3 => 13),
  array('bar' => 'baz', "foo" => 1),
  array('a' => 1,'b' => array('e' => 2,'f' => 3),'c' => array('g' => 4),'d' => 5),
);

$count = 1;
echo "\n-- Testing sort() by supplying various arrays with key values --\n";

// loop through to test sort() with different arrays,
// to test the new keys for the elements in the sorted array
foreach ($various_arrays as $array) {
  echo "\n-- Iteration $count --\n";

  echo "- With Defualt sort flag -\n";
  $temp_array = $array;
  var_dump(sort($temp_array) );
  var_dump($temp_array);

  echo "- Sort flag = SORT_REGULAR -\n";
  $temp_array = $array;
  var_dump(sort($temp_array, SORT_REGULAR) );
  var_dump($temp_array);
  $count++;
}

echo "Done\n";
?>
--EXPECT--
*** Testing sort() : usage variations ***

-- Testing sort() by supplying various arrays with key values --

-- Iteration 1 --
- With Defualt sort flag -
bool(true)
array(5) {
  [0]=>
  int(11)
  [1]=>
  int(22)
  [2]=>
  int(33)
  [3]=>
  int(55)
  [4]=>
  int(66)
}
- Sort flag = SORT_REGULAR -
bool(true)
array(5) {
  [0]=>
  int(11)
  [1]=>
  int(22)
  [2]=>
  int(33)
  [3]=>
  int(55)
  [4]=>
  int(66)
}

-- Iteration 2 --
- With Defualt sort flag -
bool(true)
array(3) {
  [0]=>
  array(3) {
    [0]=>
    string(5) "first"
    [5]=>
    string(6) "second"
    [6]=>
    string(5) "third"
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
}
- Sort flag = SORT_REGULAR -
bool(true)
array(3) {
  [0]=>
  array(3) {
    [0]=>
    string(5) "first"
    [5]=>
    string(6) "second"
    [6]=>
    string(5) "third"
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
}

-- Iteration 3 --
- With Defualt sort flag -
bool(true)
array(6) {
  [0]=>
  int(1)
  [1]=>
  int(1)
  [2]=>
  int(1)
  [3]=>
  int(1)
  [4]=>
  int(13)
  [5]=>
  int(19)
}
- Sort flag = SORT_REGULAR -
bool(true)
array(6) {
  [0]=>
  int(1)
  [1]=>
  int(1)
  [2]=>
  int(1)
  [3]=>
  int(1)
  [4]=>
  int(13)
  [5]=>
  int(19)
}

-- Iteration 4 --
- With Defualt sort flag -
bool(true)
array(2) {
  [0]=>
  string(3) "baz"
  [1]=>
  int(1)
}
- Sort flag = SORT_REGULAR -
bool(true)
array(2) {
  [0]=>
  string(3) "baz"
  [1]=>
  int(1)
}

-- Iteration 5 --
- With Defualt sort flag -
bool(true)
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(5)
  [2]=>
  array(1) {
    ["g"]=>
    int(4)
  }
  [3]=>
  array(2) {
    ["e"]=>
    int(2)
    ["f"]=>
    int(3)
  }
}
- Sort flag = SORT_REGULAR -
bool(true)
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(5)
  [2]=>
  array(1) {
    ["g"]=>
    int(4)
  }
  [3]=>
  array(2) {
    ["e"]=>
    int(2)
    ["f"]=>
    int(3)
  }
}
Done
