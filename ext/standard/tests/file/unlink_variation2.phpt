--TEST--
Test unlink() function : usage variations - unlink file in use
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip only on Linux');
}
?>
--FILE--
<?php
/* Prototype : bool unlink ( string $filename [, resource $context] );
   Description : Deletes filename
*/

/* Try to unlink file when file handle is still in use */

$file_path = dirname(__FILE__);

echo "*** Testing unlink() on a file which is in use ***\n";
// temp file name used here
$filename = "$file_path/unlink_variation2.tmp";

// create file
$fp = fopen($filename, "w");
// try unlink() on $filename
var_dump( unlink($filename) );  // expected: true on linux
var_dump( file_exists($filename) );  // confirm file is deleted
// now close file handle
fclose($fp);

echo "Done\n";
?>
--EXPECTF--
*** Testing unlink() on a file which is in use ***
bool(true)
bool(false)
Done
