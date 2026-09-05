--TEST--
Test chr() function : error conditions
--FILE--
<?php

echo "*** Testing chr() : error conditions ***\n";

echo "\n-- Testing chr() function with no arguments --\n";
try {
    var_dump( chr() );
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo "\n-- Testing chr() function with more than expected no. of arguments --\n";
$extra_arg = 10;
try {
    var_dump( chr(72, $extra_arg) );
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
*** Testing chr() : error conditions ***

-- Testing chr() function with no arguments --
ArgumentCountError: chr() expects exactly 1 argument, 0 given

-- Testing chr() function with more than expected no. of arguments --
ArgumentCountError: chr() expects exactly 1 argument, 2 given
