--TEST--
Test array_merge() function
--INI--
precision=14
--FILE--
<?php
/* Prototype: array array_merge(array $array1 [, array $array2 [, array $...]]);
   Description: Merge one or more arrays
*/

echo "\n*** Testing array_merge() basic functionality ***";
$begin_array = array(
  array(),
  array( 1 => "string"),
  array( "" => "string"),
  array( -2.44444 => 12),
  array( "a" => 1, "b" => -2.344, "b" => "string", "c" => NULL,	"d" => -2.344),
  array( 4 => 1, 3 => -2.344, "3" => "string", "2" => NULL,1 => -2.344),
  array( NULL, 1.23 => "Hi", "string" => "hello", 
  array("" => "World", "-2.34" => "a", "0" => "b"))
);

$end_array   = array(
  array(),
  array( 1 => "string"),
  array( "" => "string"),
  array( -2.44444 => 12),
  array( "a" => 1, "b" => -2.344, "b" => "string", "c" => NULL, "d" => -2.344),
  array( 4 => 1, 3 => -2.344, "3" => "string", "2" => NULL, 1=> -2.344), 
  array( NULL, 1.23 => "Hi", "string" => "hello", 
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
    print_r($result);
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

echo "\n*** Testing error conditions ***";
/* Invalid argumens */
var_dump(array_merge());
var_dump(array_merge(100, 200));
var_dump(array_merge($begin_array[0], $begin_array[1], 100));
var_dump(array_merge($begin_array[0], $begin_array[1], $arr4));

echo "Done\n";
?> 
--EXPECTF--
*** Testing array_merge() basic functionality ***

--- Iteration 0 ---
-- Inner iteration 0 of Iteration 0 --
Array
(
)

-- Inner iteration 1 of Iteration 0 --
Array
(
    [0] => string
)

-- Inner iteration 2 of Iteration 0 --
Array
(
    [] => string
)

-- Inner iteration 3 of Iteration 0 --
Array
(
    [0] => 12
)

-- Inner iteration 4 of Iteration 0 --
Array
(
    [a] => 1
    [b] => string
    [c] => 
    [d] => -2.344
)

-- Inner iteration 5 of Iteration 0 --
Array
(
    [0] => 1
    [1] => string
    [2] => 
    [3] => -2.344
)

-- Inner iteration 6 of Iteration 0 --
Array
(
    [0] => 
    [1] => Hi
    [string] => hello
    [2] => Array
        (
            [] => World
            [-2.34] => a
            [0] => b
        )

)


--- Iteration 1 ---
-- Inner iteration 0 of Iteration 1 --
Array
(
    [0] => string
)

-- Inner iteration 1 of Iteration 1 --
Array
(
    [0] => string
    [1] => string
)

-- Inner iteration 2 of Iteration 1 --
Array
(
    [0] => string
    [] => string
)

-- Inner iteration 3 of Iteration 1 --
Array
(
    [0] => string
    [1] => 12
)

-- Inner iteration 4 of Iteration 1 --
Array
(
    [0] => string
    [a] => 1
    [b] => string
    [c] => 
    [d] => -2.344
)

-- Inner iteration 5 of Iteration 1 --
Array
(
    [0] => string
    [1] => 1
    [2] => string
    [3] => 
    [4] => -2.344
)

-- Inner iteration 6 of Iteration 1 --
Array
(
    [0] => string
    [1] => 
    [2] => Hi
    [string] => hello
    [3] => Array
        (
            [] => World
            [-2.34] => a
            [0] => b
        )

)


--- Iteration 2 ---
-- Inner iteration 0 of Iteration 2 --
Array
(
    [] => string
)

-- Inner iteration 1 of Iteration 2 --
Array
(
    [] => string
    [0] => string
)

-- Inner iteration 2 of Iteration 2 --
Array
(
    [] => string
)

-- Inner iteration 3 of Iteration 2 --
Array
(
    [] => string
    [0] => 12
)

-- Inner iteration 4 of Iteration 2 --
Array
(
    [] => string
    [a] => 1
    [b] => string
    [c] => 
    [d] => -2.344
)

-- Inner iteration 5 of Iteration 2 --
Array
(
    [] => string
    [0] => 1
    [1] => string
    [2] => 
    [3] => -2.344
)

-- Inner iteration 6 of Iteration 2 --
Array
(
    [] => string
    [0] => 
    [1] => Hi
    [string] => hello
    [2] => Array
        (
            [] => World
            [-2.34] => a
            [0] => b
        )

)


--- Iteration 3 ---
-- Inner iteration 0 of Iteration 3 --
Array
(
    [0] => 12
)

-- Inner iteration 1 of Iteration 3 --
Array
(
    [0] => 12
    [1] => string
)

-- Inner iteration 2 of Iteration 3 --
Array
(
    [0] => 12
    [] => string
)

-- Inner iteration 3 of Iteration 3 --
Array
(
    [0] => 12
    [1] => 12
)

-- Inner iteration 4 of Iteration 3 --
Array
(
    [0] => 12
    [a] => 1
    [b] => string
    [c] => 
    [d] => -2.344
)

-- Inner iteration 5 of Iteration 3 --
Array
(
    [0] => 12
    [1] => 1
    [2] => string
    [3] => 
    [4] => -2.344
)

-- Inner iteration 6 of Iteration 3 --
Array
(
    [0] => 12
    [1] => 
    [2] => Hi
    [string] => hello
    [3] => Array
        (
            [] => World
            [-2.34] => a
            [0] => b
        )

)


--- Iteration 4 ---
-- Inner iteration 0 of Iteration 4 --
Array
(
    [a] => 1
    [b] => string
    [c] => 
    [d] => -2.344
)

-- Inner iteration 1 of Iteration 4 --
Array
(
    [a] => 1
    [b] => string
    [c] => 
    [d] => -2.344
    [0] => string
)

-- Inner iteration 2 of Iteration 4 --
Array
(
    [a] => 1
    [b] => string
    [c] => 
    [d] => -2.344
    [] => string
)

-- Inner iteration 3 of Iteration 4 --
Array
(
    [a] => 1
    [b] => string
    [c] => 
    [d] => -2.344
    [0] => 12
)

-- Inner iteration 4 of Iteration 4 --
Array
(
    [a] => 1
    [b] => string
    [c] => 
    [d] => -2.344
)

-- Inner iteration 5 of Iteration 4 --
Array
(
    [a] => 1
    [b] => string
    [c] => 
    [d] => -2.344
    [0] => 1
    [1] => string
    [2] => 
    [3] => -2.344
)

-- Inner iteration 6 of Iteration 4 --
Array
(
    [a] => 1
    [b] => string
    [c] => 
    [d] => -2.344
    [0] => 
    [1] => Hi
    [string] => hello
    [2] => Array
        (
            [] => World
            [-2.34] => a
            [0] => b
        )

)


--- Iteration 5 ---
-- Inner iteration 0 of Iteration 5 --
Array
(
    [0] => 1
    [1] => string
    [2] => 
    [3] => -2.344
)

-- Inner iteration 1 of Iteration 5 --
Array
(
    [0] => 1
    [1] => string
    [2] => 
    [3] => -2.344
    [4] => string
)

-- Inner iteration 2 of Iteration 5 --
Array
(
    [0] => 1
    [1] => string
    [2] => 
    [3] => -2.344
    [] => string
)

-- Inner iteration 3 of Iteration 5 --
Array
(
    [0] => 1
    [1] => string
    [2] => 
    [3] => -2.344
    [4] => 12
)

-- Inner iteration 4 of Iteration 5 --
Array
(
    [0] => 1
    [1] => string
    [2] => 
    [3] => -2.344
    [a] => 1
    [b] => string
    [c] => 
    [d] => -2.344
)

-- Inner iteration 5 of Iteration 5 --
Array
(
    [0] => 1
    [1] => string
    [2] => 
    [3] => -2.344
    [4] => 1
    [5] => string
    [6] => 
    [7] => -2.344
)

-- Inner iteration 6 of Iteration 5 --
Array
(
    [0] => 1
    [1] => string
    [2] => 
    [3] => -2.344
    [4] => 
    [5] => Hi
    [string] => hello
    [6] => Array
        (
            [] => World
            [-2.34] => a
            [0] => b
        )

)


--- Iteration 6 ---
-- Inner iteration 0 of Iteration 6 --
Array
(
    [0] => 
    [1] => Hi
    [string] => hello
    [2] => Array
        (
            [] => World
            [-2.34] => a
            [0] => b
        )

)

-- Inner iteration 1 of Iteration 6 --
Array
(
    [0] => 
    [1] => Hi
    [string] => hello
    [2] => Array
        (
            [] => World
            [-2.34] => a
            [0] => b
        )

    [3] => string
)

-- Inner iteration 2 of Iteration 6 --
Array
(
    [0] => 
    [1] => Hi
    [string] => hello
    [2] => Array
        (
            [] => World
            [-2.34] => a
            [0] => b
        )

    [] => string
)

-- Inner iteration 3 of Iteration 6 --
Array
(
    [0] => 
    [1] => Hi
    [string] => hello
    [2] => Array
        (
            [] => World
            [-2.34] => a
            [0] => b
        )

    [3] => 12
)

-- Inner iteration 4 of Iteration 6 --
Array
(
    [0] => 
    [1] => Hi
    [string] => hello
    [2] => Array
        (
            [] => World
            [-2.34] => a
            [0] => b
        )

    [a] => 1
    [b] => string
    [c] => 
    [d] => -2.344
)

-- Inner iteration 5 of Iteration 6 --
Array
(
    [0] => 
    [1] => Hi
    [string] => hello
    [2] => Array
        (
            [] => World
            [-2.34] => a
            [0] => b
        )

    [3] => 1
    [4] => string
    [5] => 
    [6] => -2.344
)

-- Inner iteration 6 of Iteration 6 --
Array
(
    [0] => 
    [1] => Hi
    [string] => hello
    [2] => Array
        (
            [] => World
            [-2.34] => a
            [0] => b
        )

    [3] => 
    [4] => Hi
    [5] => Array
        (
            [] => World
            [-2.34] => a
            [0] => b
        )

)

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

*** Testing error conditions ***
Warning: array_merge() expects at least 1 parameter, 0 given in %s on line %d
NULL

Warning: array_merge(): Argument #1 is not an array in %s on line %d
NULL

Warning: array_merge(): Argument #3 is not an array in %s on line %d
NULL

Notice: Undefined variable: arr4 in %s on line %d

Warning: array_merge(): Argument #3 is not an array in %s on line %d
NULL
Done
