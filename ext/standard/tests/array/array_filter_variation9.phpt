--TEST--
Test array_filter() function : usage variations - built-in functions as 'callback' argument
--FILE--
<?php
/*
* Passing built-in functions and different language constructs as 'callback' argument
*/

echo "*** Testing array_filter() : usage variations - built-in functions as 'callback' argument ***\n";

$input = array(0, 1, -1, 10, 100, 1000, null);

// using built-in function 'is_int' as 'callback'
var_dump( array_filter($input, 'is_int') );

// using built-in function 'chr' as 'callback'
var_dump( array_filter($input, 'chr') );

// using language construct 'echo' as 'callback'
try {
    var_dump( array_filter($input, 'echo') );
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

// using language construct 'exit' as 'callback'
try {
    var_dump( array_filter($input, 'exit') );
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

echo "Done"
?>
--EXPECT--
*** Testing array_filter() : usage variations - built-in functions as 'callback' argument ***
array(6) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(-1)
  [3]=>
  int(10)
  [4]=>
  int(100)
  [5]=>
  int(1000)
}
array(7) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(-1)
  [3]=>
  int(10)
  [4]=>
  int(100)
  [5]=>
  int(1000)
  [6]=>
  NULL
}
array_filter(): Argument #2 ($callback) must be a valid callback, function "echo" not found or invalid function name
array_filter(): Argument #2 ($callback) must be a valid callback, function "exit" not found or invalid function name
Done
