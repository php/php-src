--TEST--
Test fread() function : error conditions
--FILE--
<?php
echo "*** Testing error conditions ***\n";
$filename = __FILE__;
$file_handle = fopen($filename, "r");

// invalid length argument
echo "-- Testing fread() with invalid length arguments --\n";
$len = 0;
try {
    var_dump( fread($file_handle, $len) );
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

$len = -10;
try {
    var_dump( fread($file_handle, $len) );
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
*** Testing error conditions ***
-- Testing fread() with invalid length arguments --
fread(): Argument #2 ($length) must be greater than 0
fread(): Argument #2 ($length) must be greater than 0
