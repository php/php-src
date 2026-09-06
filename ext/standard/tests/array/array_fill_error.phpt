--TEST--
Test array_fill() function : error conditions
--FILE--
<?php
echo "*** Testing array_fill() : error conditions ***\n";

// calling array_fill with negative values for 'num' parameter
$start_key = 0;
$num = -1;
$val = 1;

try {
    var_dump( array_fill($start_key,$num,$val) );
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
*** Testing array_fill() : error conditions ***
ValueError: array_fill(): Argument #2 ($count) must be greater than or equal to 0
