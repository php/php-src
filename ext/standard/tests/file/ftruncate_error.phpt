--TEST--
Test ftruncate() function : error conditions
--FILE--
<?php
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
try {
    var_dump( ftruncate($file_handle,10) );
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
// check the first size
var_dump( filesize($filename) );

echo "Done\n";
?>
--CLEAN--
<?php
$filename = __DIR__."/ftruncate_error.tmp";
unlink( $filename );
?>
--EXPECT--
*** Testing ftruncate() : error conditions ***

 Initial file size = 36
-- Testing ftruncate() with closed/unset file handle --
ftruncate(): supplied resource is not a valid stream resource
int(36)
Done
