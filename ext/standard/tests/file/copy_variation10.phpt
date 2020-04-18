--TEST--
Test copy() function: usage variations - identical names
--FILE--
<?php
/* Prototype: bool copy ( string $source, string $dest );
   Description: Makes a copy of the file source to dest.
     Returns TRUE on success or FALSE on failure.
*/

/* Test copy(): Try copying source file to desntination file, where destination file name is identical to source name */

$file_path = __DIR__;

echo "*** Test copy(): Trying to create a copy of file with the same source name ***\n";
$file = $file_path."/copy_variation10.tmp";
$file_handle =  fopen($file, "w");
fwrite($file_handle, str_repeat("Hello2world...\n", 100));
fclose($file_handle);

var_dump( copy($file, $file) );
var_dump( file_exists($file) );
var_dump( filesize($file) );

echo "*** Done ***\n";
?>
--CLEAN--
<?php
unlink(__DIR__."/copy_variation10.tmp");
?>
--EXPECT--
*** Test copy(): Trying to create a copy of file with the same source name ***
bool(false)
bool(true)
int(1500)
*** Done ***
