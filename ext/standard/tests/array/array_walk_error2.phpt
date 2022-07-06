--TEST--
Test array_walk() function : error conditions - callback parameters
--FILE--
<?php
/*
 * Testing array_walk() by passing more number of parameters to callback function
 */
$input = array(1);

function callback1($value, $key, $user_data ) {
  echo "\ncallback1() invoked \n";
}

function callback2($value, $key, $user_data1, $user_data2) {
  echo "\ncallback2() invoked \n";
}
echo "*** Testing array_walk() : error conditions - callback parameters ***\n";

// expected: Missing argument Warning
try {
    var_dump( array_walk($input, "callback1") );
} catch (Throwable $e) {
    echo "Exception: " . $e->getMessage() . "\n";
}
try {
    var_dump( array_walk($input, "callback2", 4) );
} catch (Throwable $e) {
    echo "Exception: " . $e->getMessage() . "\n";
}

// expected: Warning is suppressed
try {
    var_dump( @array_walk($input, "callback1") );
} catch (Throwable $e) {
    echo "Exception: " . $e->getMessage() . "\n";
}
try {
    var_dump( @array_walk($input, "callback2", 4) );
} catch (Throwable $e) {
    echo "Exception: " . $e->getMessage() . "\n";
}

echo "-- Testing array_walk() function with too many callback parameters --\n";
try {
    var_dump( array_walk($input, "callback1", 20, 10) );
} catch (Throwable $e) {
    echo "Exception: " . $e->getMessage() . "\n";
}

echo "Done";
?>
--EXPECT--
*** Testing array_walk() : error conditions - callback parameters ***
Exception: Too few arguments to function callback1(), 2 passed and exactly 3 expected
Exception: Too few arguments to function callback2(), 3 passed and exactly 4 expected
Exception: Too few arguments to function callback1(), 2 passed and exactly 3 expected
Exception: Too few arguments to function callback2(), 3 passed and exactly 4 expected
-- Testing array_walk() function with too many callback parameters --
Exception: array_walk() expects at most 3 arguments, 4 given
Done
