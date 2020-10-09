--TEST--
Test fgetc() function : usage variations - closed handle
--FILE--
<?php
/* try reading a char using fgetc() using invalid handles
    - closed file handle
    - unset file handle
*/

// include the header for common test function
include ("file.inc");

echo "*** Testing fgetc() : usage variations ***\n";

echo "-- Testing fgetc() with closed handle --\n";
// open the file for reading
$file_handle = fopen(__FILE__, "r");
// close the file
fclose($file_handle);

// read from closed file
try {
    var_dump( fgetc($file_handle) );
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

echo "Done";
?>
--EXPECT--
*** Testing fgetc() : usage variations ***
-- Testing fgetc() with closed handle --
fgetc(): supplied resource is not a valid stream resource
Done
