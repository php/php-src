--TEST--
Test fread() function : error conditions
--FILE--
<?php
/*
 Prototype: string fread ( resource $handle [, int $length] );
 Description: reads up to length bytes from the file pointer referenced by handle.
   Reading stops when up to length bytes have been read, EOF (end of file) is
   reached, (for network streams) when a packet becomes available, or (after
   opening userspace stream) when 8192 bytes have been read whichever comes first.
*/

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

--EXPECT--
*** Testing error conditions ***
-- Testing fread() with invalid length arguments --
Length parameter must be greater than 0
Length parameter must be greater than 0
