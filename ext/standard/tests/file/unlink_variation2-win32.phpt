--TEST--
Test unlink() function : usage variations - unlink file in use
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
    die('skip only on Windows');
}
?>
--FILE--
<?php
/* Prototype : bool unlink ( string $filename [, resource $context] );
   Description : Deletes filename
*/

/* Try to unlink file when file handle is still in use */

$file_path = dirname(__FILE__);

echo "*** Testing unlink() on a file when file handle is open ***\n";
// temp file name used here
$filename = "$file_path/unlink_variation2.tmp";

// create file
$fp = fopen($filename, "w");
// try unlink() on $filename
var_dump( unlink($filename) );  // expected: false as file handle is still open
// now close file handle
fclose($fp);

// now unlink file
var_dump( unlink($filename) );  // expected: true
var_dump( file_exists($filename) );  // confirm file is deleted

echo "Done\n";
?>
--EXPECTF--
*** Testing unlink() on a file when file handle is open ***

Warning: unlink(%s): %s in %s on line %d
bool(false)
bool(true)
bool(false)
Done
