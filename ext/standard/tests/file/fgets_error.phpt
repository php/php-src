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
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

$len = -10;
try {
    var_dump( fgets($fp, $len) );
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
$len = 1;
var_dump( fgets($fp, $len) ); // return length - 1 always, expect false

?>
--EXPECT--
*** Testing error conditions ***
-- Testing fgets() with invalid length arguments --
fgets(): Argument #2 ($length) must be greater than 0
fgets(): Argument #2 ($length) must be greater than 0
bool(false)
