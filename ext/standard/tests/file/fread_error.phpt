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
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

$len = -10;
try {
    var_dump( fread($file_handle, $len) );
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
*** Testing error conditions ***
-- Testing fread() with invalid length arguments --
ValueError: fread(): Argument #2 ($length) must be greater than 0
ValueError: fread(): Argument #2 ($length) must be greater than 0
