--TEST--
Test fgets() function : error conditions
--FILE--
<?php
echo "*** Testing error conditions ***\n";

$fp = fopen(__FILE__, "r");

// invalid length argument
echo "-- Testing fgets() with invalid length arguments --\n";
$len = 0;
try {
    var_dump( fgets($fp, $len) );
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

$len = -10;
try {
    var_dump( fgets($fp, $len) );
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
$len = 1;
var_dump( fgets($fp, $len) ); // return length - 1 always, expect false

?>
--EXPECT--
*** Testing error conditions ***
-- Testing fgets() with invalid length arguments --
ValueError: fgets(): Argument #2 ($length) must be greater than 0
ValueError: fgets(): Argument #2 ($length) must be greater than 0
bool(false)
