--TEST--
Test unlink() function : usage variations - unlink links
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip only on Linux');
}
?>
--FILE--
<?php

/* Delete link files - soft and hard links */

$file_path = __DIR__;
// temp file used
$filename = "$file_path/unlink_variation3.tmp";

echo "*** Testing unlink() on soft and hard links ***\n";
// create temp file
$fp = fopen($filename, "w");
fclose($fp);
// link name used here
$linkname = "$file_path/unlink_variation3_link.tmp";

echo "-- Testing unlink() on soft link --\n";
// create soft link
var_dump( symlink($filename, $linkname) );  // expected: true
// unlink soft link
var_dump( unlink($linkname) );  // expected: true
var_dump( file_exists($linkname) );  // confirm link is deleted

echo "-- Testing unlink() on hard link --\n";
// create hard link
var_dump( link($filename, $linkname) );  // expected: true
// delete hard link
var_dump( unlink($linkname) );  // expected: true
var_dump( file_exists($linkname) );  // confirm link is deleted

// delete temp file
var_dump( unlink($filename) );
var_dump( file_exists($filename) );  // confirm file is deleted

echo "Done\n";
?>
--EXPECT--
*** Testing unlink() on soft and hard links ***
-- Testing unlink() on soft link --
bool(true)
bool(true)
bool(false)
-- Testing unlink() on hard link --
bool(true)
bool(true)
bool(false)
bool(true)
bool(false)
Done
