--TEST--
Test fgets() function : usage variations - closed handle
--FILE--
<?php
/* try reading a line using fgets() using invalid handles
    - closed file handle
    - unset file handle
*/

// include the header for common test function
include ("file.inc");

echo "*** Testing fgets() : usage variations ***\n";

echo "-- Testing fgets() with closed handle --\n";
// open the file for reading
$file_handle = fopen(__FILE__, "r");
// close the file
fclose($file_handle);

// read from closed file
try {
    var_dump( fgets($file_handle) ); // default length
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump( fgets($file_handle, 10) ); // with specific length
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

echo "Done";
?>
--EXPECT--
*** Testing fgets() : usage variations ***
-- Testing fgets() with closed handle --
fgets(): supplied resource is not a valid stream resource
fgets(): supplied resource is not a valid stream resource
Done
