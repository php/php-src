--TEST--
Test ftruncate() function : error conditions
--FILE--
<?php
/*
 Prototype: bool ftruncate ( resource $handle, int $size );
 Description: truncates a file to a given length
*/

echo "*** Testing ftruncate() : error conditions ***\n";

$filename = __DIR__."/ftruncate_error.tmp";
$file_handle = fopen($filename, "w" );
fwrite($file_handle, "Testing ftruncate error conditions \n");
fflush($file_handle);
echo "\n Initial file size = ".filesize($filename)."\n";

// ftruncate() on a file handle which is already closed/unset
echo "-- Testing ftruncate() with closed/unset file handle --\n";

// ftruncate on close file handle
fclose($file_handle);
var_dump( ftruncate($file_handle,10) );
// check the first size
var_dump( filesize($filename) );

echo "Done\n";
?>
--CLEAN--
<?php
$filename = __DIR__."/ftruncate_error.tmp";
unlink( $filename );
?>
--EXPECTF--
*** Testing ftruncate() : error conditions ***

 Initial file size = 36
-- Testing ftruncate() with closed/unset file handle --

Warning: ftruncate(): supplied resource is not a valid stream resource in %s on line %d
bool(false)
int(36)
Done
