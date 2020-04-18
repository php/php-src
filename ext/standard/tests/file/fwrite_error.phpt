--TEST--
Test fwrite() function : error conditions
--FILE--
<?php
/*
 Prototype: int fwrite ( resource $handle,string string, [, int $length] );
 Description: fwrite() writes the contents of string to the file stream pointed to by handle.
              If the length arquement is given,writing will stop after length bytes have been
              written or the end of string reached, whichever comes first.
              fwrite() returns the number of bytes written or FALSE on error
*/

// include the file.inc for Function: function delete_file($filename)
include ("file.inc");

echo "*** Testing fwrite() : error conditions ***\n";

$filename = __DIR__."/fwrite_error.tmp";
$file_handle  = fopen ( $filename, "w");
$data = "data";

// invalid length argument
echo "-- Testing fwrite() with invalid length arguments --\n";
$len = 0;
var_dump( fwrite($file_handle, $data, $len) );
$len = -10;
var_dump( fwrite($file_handle, $data, $len) );

// fwrite() on a file handle which is already closed
echo "-- Testing fwrite() with closed/unset file handle --\n";
fclose($file_handle);
try {
    var_dump(fwrite($file_handle,"data"));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

echo "Done\n";
?>
--CLEAN--
<?php
$filename = __DIR__."/fwrite_error.tmp";
unlink( $filename );
?>
--EXPECT--
*** Testing fwrite() : error conditions ***
-- Testing fwrite() with invalid length arguments --
int(0)
int(0)
-- Testing fwrite() with closed/unset file handle --
fwrite(): supplied resource is not a valid stream resource
Done
