--TEST--
Test array_merge() function
--INI--
precision=14
--FILE--
<?php
echo "\n*** Testing array_merge() basic functionality ***";
$begin_array = array(
  array(),
  array( 1 => "string"),
  array( "" => "string"),
  array( -2 => 12),
  array( "a" => 1, "b" => -2.344, "b" => "string", "c" => NULL,	"d" => -2.344),
  array( 4 => 1, 3 => -2.344, "3" => "string", "2" => NULL,1 => -2.344),
  array( NULL, 1 => "Hi", "string" => "hello",
  array("" => "World", "-2.34" => "a", "0" => "b"))
);

$end_array   = array(
  array(),
  array( 1 => "string"),
  array( "" => "string"),
  array( -2 => 12),
  array( "a" => 1, "b" => -2.344, "b" => "string", "c" => NULL, "d" => -2.344),
  array( 4 => 1, 3 => -2.344, "3" => "string", "2" => NULL, 1=> -2.344),
  array( NULL, 1 => "Hi", "string" => "hello",
         array("" => "World", "-2.34" => "a", "0" => "b"))
);

/* loop through to merge two arrays */
$count_outer = 0;
foreach($begin_array as $first) {
  echo "\n\n--- Iteration $count_outer ---";
  $count_inner = 0;
  foreach($end_array as $second) {
    echo "\n-- Inner iteration $count_inner of Iteration $count_outer --\n";
    $result = array_merge($first, $second);
    var_dump($result);
    $count_inner++;
  }
  $count_outer++;
}


echo "\n*** Testing array_merge() with three or more arrays ***\n";
var_dump( array_merge( $end_array[0],
                       $end_array[5],
                       $end_array[4],
                       $end_array[6]
                     )
        );

var_dump( array_merge( $end_array[0],
                       $end_array[5],
                       array("array on fly"),
                       array("nullarray" => array())
                     )
        );


echo "\n*** Testing single array argument ***\n";
/* Empty array */
var_dump(array_merge(array()));

/* associative array with string keys, which will not be re-indexed */
var_dump(array_merge($begin_array[4]));

/* associative array with numeric keys, which will be re-indexed */
var_dump(array_merge($begin_array[5]));

/* associative array with mixed keys and sub-array as element */
var_dump(array_merge($begin_array[6]));

echo "\n*** Testing array_merge() with typecasting non-array to array ***\n";
var_dump(array_merge($begin_array[4], (array)"type1", (array)10, (array)12.34));

echo "\n*** Testing array_merge without any arguments ***\n";
var_dump(array_merge());

echo "Done\n";
?>
--EXPECT--
*** Testing array_merge() basic functionality ***

--- Iteration 0 ---
-- Inner iteration 0 of Iteration 0 --
array(0) {
}

-- Inner iteration 1 of Iteration 0 --
array(1) {
  [0]=>
  string(6) "string"
}

-- Inner iteration 2 of Iteration 0 --
array(1) {
  [""]=>
  string(6) "string"
}

-- Inner iteration 3 of Iteration 0 --
array(1) {
  [0]=>
  int(12)
}

-- Inner iteration 4 of Iteration 0 --
array(4) {
  ["a"]=>
  int(1)
  ["b"]=>
  string(6) "string"
  ["c"]=>
  NULL
  ["d"]=>
  float(-2.344)
}

-- Inner iteration 5 of Iteration 0 --
array(4) {
  [0]=>
  int(1)
  [1]=>
  string(6) "string"
  [2]=>
  NULL
  [3]=>
  float(-2.344)
}

-- Inner iteration 6 of Iteration 0 --
array(4) {
  [0]=>
  NULL
  [1]=>
  string(2) "Hi"
  ["string"]=>
  string(5) "hello"
  [2]=>
  array(3) {
    [""]=>
    string(5) "World"
    ["-2.34"]=>
    string(1) "a"
    [0]=>
    string(1) "b"
  }
}


--- Iteration 1 ---
-- Inner iteration 0 of Iteration 1 --
array(1) {
  [0]=>
  string(6) "string"
}

-- Inner iteration 1 of Iteration 1 --
array(2) {
  [0]=>
  string(6) "string"
  [1]=>
  string(6) "string"
}

-- Inner iteration 2 of Iteration 1 --
array(2) {
  [0]=>
  string(6) "string"
  [""]=>
  string(6) "string"
}

-- Inner iteration 3 of Iteration 1 --
array(2) {
  [0]=>
  string(6) "string"
  [1]=>
  int(12)
}

-- Inner iteration 4 of Iteration 1 --
array(5) {
  [0]=>
  string(6) "string"
  ["a"]=>
  int(1)
  ["b"]=>
  string(6) "string"
  ["c"]=>
  NULL
  ["d"]=>
  float(-2.344)
}

-- Inner iteration 5 of Iteration 1 --
array(5) {
  [0]=>
  string(6) "string"
  [1]=>
  int(1)
  [2]=>
  string(6) "string"
  [3]=>
  NULL
  [4]=>
  float(-2.344)
}

-- Inner iteration 6 of Iteration 1 --
array(5) {
  [0]=>
  string(6) "string"
  [1]=>
  NULL
  [2]=>
  string(2) "Hi"
  ["string"]=>
  string(5) "hello"
  [3]=>
  array(3) {
    [""]=>
    string(5) "World"
    ["-2.34"]=>
    string(1) "a"
    [0]=>
    string(1) "b"
  }
}


--- Iteration 2 ---
-- Inner iteration 0 of Iteration 2 --
array(1) {
  [""]=>
  string(6) "string"
}

-- Inner iteration 1 of Iteration 2 --
array(2) {
  [""]=>
  string(6) "string"
  [0]=>
  string(6) "string"
}

-- Inner iteration 2 of Iteration 2 --
array(1) {
  [""]=>
  string(6) "string"
}

-- Inner iteration 3 of Iteration 2 --
array(2) {
  [""]=>
  string(6) "string"
  [0]=>
  int(12)
}

-- Inner iteration 4 of Iteration 2 --
array(5) {
  [""]=>
  string(6) "string"
  ["a"]=>
  int(1)
  ["b"]=>
  string(6) "string"
  ["c"]=>
  NULL
  ["d"]=>
  float(-2.344)
}

-- Inner iteration 5 of Iteration 2 --
array(5) {
  [""]=>
  string(6) "string"
  [0]=>
  int(1)
  [1]=>
  string(6) "string"
  [2]=>
  NULL
  [3]=>
  float(-2.344)
}

-- Inner iteration 6 of Iteration 2 --
array(5) {
  [""]=>
  string(6) "string"
  [0]=>
  NULL
  [1]=>
  string(2) "Hi"
  ["string"]=>
  string(5) "hello"
  [2]=>
  array(3) {
    [""]=>
    string(5) "World"
    ["-2.34"]=>
    string(1) "a"
    [0]=>
    string(1) "b"
  }
}


--- Iteration 3 ---
-- Inner iteration 0 of Iteration 3 --
array(1) {
  [0]=>
  int(12)
}

-- Inner iteration 1 of Iteration 3 --
array(2) {
  [0]=>
  int(12)
  [1]=>
  string(6) "string"
}

-- Inner iteration 2 of Iteration 3 --
array(2) {
  [0]=>
  int(12)
  [""]=>
  string(6) "string"
}

-- Inner iteration 3 of Iteration 3 --
array(2) {
  [0]=>
  int(12)
  [1]=>
  int(12)
}

-- Inner iteration 4 of Iteration 3 --
array(5) {
  [0]=>
  int(12)
  ["a"]=>
  int(1)
  ["b"]=>
  string(6) "string"
  ["c"]=>
  NULL
  ["d"]=>
  float(-2.344)
}

-- Inner iteration 5 of Iteration 3 --
array(5) {
  [0]=>
  int(12)
  [1]=>
  int(1)
  [2]=>
  string(6) "string"
  [3]=>
  NULL
  [4]=>
  float(-2.344)
}

-- Inner iteration 6 of Iteration 3 --
array(5) {
  [0]=>
  int(12)
  [1]=>
  NULL
  [2]=>
  string(2) "Hi"
  ["string"]=>
  string(5) "hello"
  [3]=>
  array(3) {
    [""]=>
    string(5) "World"
    ["-2.34"]=>
    string(1) "a"
    [0]=>
    string(1) "b"
  }
}


--- Iteration 4 ---
-- Inner iteration 0 of Iteration 4 --
array(4) {
  ["a"]=>
  int(1)
  ["b"]=>
  string(6) "string"
  ["c"]=>
  NULL
  ["d"]=>
  float(-2.344)
}

-- Inner iteration 1 of Iteration 4 --
array(5) {
  ["a"]=>
  int(1)
  ["b"]=>
  string(6) "string"
  ["c"]=>
  NULL
  ["d"]=>
  float(-2.344)
  [0]=>
  string(6) "string"
}

-- Inner iteration 2 of Iteration 4 --
array(5) {
  ["a"]=>
  int(1)
  ["b"]=>
  string(6) "string"
  ["c"]=>
  NULL
  ["d"]=>
  float(-2.344)
  [""]=>
  string(6) "string"
}

-- Inner iteration 3 of Iteration 4 --
array(5) {
  ["a"]=>
  int(1)
  ["b"]=>
  string(6) "string"
  ["c"]=>
  NULL
  ["d"]=>
  float(-2.344)
  [0]=>
  int(12)
}

-- Inner iteration 4 of Iteration 4 --
array(4) {
  ["a"]=>
  int(1)
  ["b"]=>
  string(6) "string"
  ["c"]=>
  NULL
  ["d"]=>
  float(-2.344)
}

-- Inner iteration 5 of Iteration 4 --
array(8) {
  ["a"]=>
  int(1)
  ["b"]=>
  string(6) "string"
  ["c"]=>
  NULL
  ["d"]=>
  float(-2.344)
  [0]=>
  int(1)
  [1]=>
  string(6) "string"
  [2]=>
  NULL
  [3]=>
  float(-2.344)
}

-- Inner iteration 6 of Iteration 4 --
array(8) {
  ["a"]=>
  int(1)
  ["b"]=>
  string(6) "string"
  ["c"]=>
  NULL
  ["d"]=>
  float(-2.344)
  [0]=>
  NULL
  [1]=>
  string(2) "Hi"
  ["string"]=>
  string(5) "hello"
  [2]=>
  array(3) {
    [""]=>
    string(5) "World"
    ["-2.34"]=>
    string(1) "a"
    [0]=>
    string(1) "b"
  }
}


--- Iteration 5 ---
-- Inner iteration 0 of Iteration 5 --
array(4) {
  [0]=>
  int(1)
  [1]=>
  string(6) "string"
  [2]=>
  NULL
  [3]=>
  float(-2.344)
}

-- Inner iteration 1 of Iteration 5 --
array(5) {
  [0]=>
  int(1)
  [1]=>
  string(6) "string"
  [2]=>
  NULL
  [3]=>
  float(-2.344)
  [4]=>
  string(6) "string"
}

-- Inner iteration 2 of Iteration 5 --
array(5) {
  [0]=>
  int(1)
  [1]=>
  string(6) "string"
  [2]=>
  NULL
  [3]=>
  float(-2.344)
  [""]=>
  string(6) "string"
}

-- Inner iteration 3 of Iteration 5 --
array(5) {
  [0]=>
  int(1)
  [1]=>
  string(6) "string"
  [2]=>
  NULL
  [3]=>
  float(-2.344)
  [4]=>
  int(12)
}

-- Inner iteration 4 of Iteration 5 --
array(8) {
  [0]=>
  int(1)
  [1]=>
  string(6) "string"
  [2]=>
  NULL
  [3]=>
  float(-2.344)
  ["a"]=>
  int(1)
  ["b"]=>
  string(6) "string"
  ["c"]=>
  NULL
  ["d"]=>
  float(-2.344)
}

-- Inner iteration 5 of Iteration 5 --
array(8) {
  [0]=>
  int(1)
  [1]=>
  string(6) "string"
  [2]=>
  NULL
  [3]=>
  float(-2.344)
  [4]=>
  int(1)
  [5]=>
  string(6) "string"
  [6]=>
  NULL
  [7]=>
  float(-2.344)
}

-- Inner iteration 6 of Iteration 5 --
array(8) {
  [0]=>
  int(1)
  [1]=>
  string(6) "string"
  [2]=>
  NULL
  [3]=>
  float(-2.344)
  [4]=>
  NULL
  [5]=>
  string(2) "Hi"
  ["string"]=>
  string(5) "hello"
  [6]=>
  array(3) {
    [""]=>
    string(5) "World"
    ["-2.34"]=>
    string(1) "a"
    [0]=>
    string(1) "b"
  }
}


--- Iteration 6 ---
-- Inner iteration 0 of Iteration 6 --
array(4) {
  [0]=>
  NULL
  [1]=>
  string(2) "Hi"
  ["string"]=>
  string(5) "hello"
  [2]=>
  array(3) {
    [""]=>
    string(5) "World"
    ["-2.34"]=>
    string(1) "a"
    [0]=>
    string(1) "b"
  }
}

-- Inner iteration 1 of Iteration 6 --
array(5) {
  [0]=>
  NULL
  [1]=>
  string(2) "Hi"
  ["string"]=>
  string(5) "hello"
  [2]=>
  array(3) {
    [""]=>
    string(5) "World"
    ["-2.34"]=>
    string(1) "a"
    [0]=>
    string(1) "b"
  }
  [3]=>
  string(6) "string"
}

-- Inner iteration 2 of Iteration 6 --
array(5) {
  [0]=>
  NULL
  [1]=>
  string(2) "Hi"
  ["string"]=>
  string(5) "hello"
  [2]=>
  array(3) {
    [""]=>
    string(5) "World"
    ["-2.34"]=>
    string(1) "a"
    [0]=>
    string(1) "b"
  }
  [""]=>
  string(6) "string"
}

-- Inner iteration 3 of Iteration 6 --
array(5) {
  [0]=>
  NULL
  [1]=>
  string(2) "Hi"
  ["string"]=>
  string(5) "hello"
  [2]=>
  array(3) {
    [""]=>
    string(5) "World"
    ["-2.34"]=>
    string(1) "a"
    [0]=>
    string(1) "b"
  }
  [3]=>
  int(12)
}

-- Inner iteration 4 of Iteration 6 --
array(8) {
  [0]=>
  NULL
  [1]=>
  string(2) "Hi"
  ["string"]=>
  string(5) "hello"
  [2]=>
  array(3) {
    [""]=>
    string(5) "World"
    ["-2.34"]=>
    string(1) "a"
    [0]=>
    string(1) "b"
  }
  ["a"]=>
  int(1)
  ["b"]=>
  string(6) "string"
  ["c"]=>
  NULL
  ["d"]=>
  float(-2.344)
}

-- Inner iteration 5 of Iteration 6 --
array(8) {
  [0]=>
  NULL
  [1]=>
  string(2) "Hi"
  ["string"]=>
  string(5) "hello"
  [2]=>
  array(3) {
    [""]=>
    string(5) "World"
    ["-2.34"]=>
    string(1) "a"
    [0]=>
    string(1) "b"
  }
  [3]=>
  int(1)
  [4]=>
  string(6) "string"
  [5]=>
  NULL
  [6]=>
  float(-2.344)
}

-- Inner iteration 6 of Iteration 6 --
array(7) {
  [0]=>
  NULL
  [1]=>
  string(2) "Hi"
  ["string"]=>
  string(5) "hello"
  [2]=>
  array(3) {
    [""]=>
    string(5) "World"
    ["-2.34"]=>
    string(1) "a"
    [0]=>
    string(1) "b"
  }
  [3]=>
  NULL
  [4]=>
  string(2) "Hi"
  [5]=>
  array(3) {
    [""]=>
    string(5) "World"
    ["-2.34"]=>
    string(1) "a"
    [0]=>
    string(1) "b"
  }
}

*** Testing array_merge() with three or more arrays ***
array(12) {
  [0]=>
  int(1)
  [1]=>
  string(6) "string"
  [2]=>
  NULL
  [3]=>
  float(-2.344)
  ["a"]=>
  int(1)
  ["b"]=>
  string(6) "string"
  ["c"]=>
  NULL
  ["d"]=>
  float(-2.344)
  [4]=>
  NULL
  [5]=>
  string(2) "Hi"
  ["string"]=>
  string(5) "hello"
  [6]=>
  array(3) {
    [""]=>
    string(5) "World"
    ["-2.34"]=>
    string(1) "a"
    [0]=>
    string(1) "b"
  }
}
array(6) {
  [0]=>
  int(1)
  [1]=>
  string(6) "string"
  [2]=>
  NULL
  [3]=>
  float(-2.344)
  [4]=>
  string(12) "array on fly"
  ["nullarray"]=>
  array(0) {
  }
}

*** Testing single array argument ***
array(0) {
}
array(4) {
  ["a"]=>
  int(1)
  ["b"]=>
  string(6) "string"
  ["c"]=>
  NULL
  ["d"]=>
  float(-2.344)
}
array(4) {
  [0]=>
  int(1)
  [1]=>
  string(6) "string"
  [2]=>
  NULL
  [3]=>
  float(-2.344)
}
array(4) {
  [0]=>
  NULL
  [1]=>
  string(2) "Hi"
  ["string"]=>
  string(5) "hello"
  [2]=>
  array(3) {
    [""]=>
    string(5) "World"
    ["-2.34"]=>
    string(1) "a"
    [0]=>
    string(1) "b"
  }
}

*** Testing array_merge() with typecasting non-array to array ***
array(7) {
  ["a"]=>
  int(1)
  ["b"]=>
  string(6) "string"
  ["c"]=>
  NULL
  ["d"]=>
  float(-2.344)
  [0]=>
  string(5) "type1"
  [1]=>
  int(10)
  [2]=>
  float(12.34)
}

*** Testing array_merge without any arguments ***
array(0) {
}
Done
