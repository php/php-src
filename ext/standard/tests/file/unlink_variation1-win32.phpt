--TEST--
Test unlink() function : usage variations - unlinking file in a directory
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

/* Delete file having default permission but its dir having readonly permission
   Delete file having readonly permission but dir having default permission
*/


$file_path = __DIR__;

// temp dir name used here
$dirname = "$file_path/unlink_variation1";
// temp filename used here
$filename = "$dirname/unlink_variation1-win32.tmp";

echo "\n*** Testing unlink() on file inside a directory ***\n";
// create temp dir
mkdir($dirname);
// create temp file inside $dirname
$fp = fopen($filename, "w");
fclose($fp);

echo "-- Unlink file having default permission and its dir having read only permission --\n";
// remove write permission of $dirname
// on windows dir permission is not respected
var_dump( chmod($dirname, 0444) );
// now try deleting $filename
var_dump( unlink($filename) );  // expected: true
var_dump( file_exists($filename) );  // confirm file is deleted

// remove the dir
var_dump( chmod($dirname, 0777) );
rmdir($dirname);

echo "\n-- Unlinking file without write permission, its dir having default permission --\n";
// create the temp dir
mkdir($dirname);

// create the temp file
$fp = fopen($filename, "w");
fclose($fp);

// remove write permission from file
var_dump( chmod($filename, 0444) );

// now try deleting temp file inside $dirname
var_dump( unlink($filename) );  // expected: false

// reassign write permission to temp file
chmod($filename, 0777);
// delete temp file
var_dump( unlink($filename) );
var_dump( file_exists($filename) );  // confirm file is deleted
// remove temp dir
rmdir($dirname);

echo "Done\n";
?>
--EXPECTF--
*** Testing unlink() on file inside a directory ***
-- Unlink file having default permission and its dir having read only permission --
bool(true)
bool(true)
bool(false)
bool(true)

-- Unlinking file without write permission, its dir having default permission --
bool(true)

Warning: unlink(%s/unlink_variation1/unlink_variation1-win32.tmp): Permission denied in %s on line %d
bool(false)
bool(true)
bool(false)
Done
