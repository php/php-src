--TEST--
Test dir() function : usage variations - non-existent directory
--FILE--
<?php
/* 
 * Prototype  : object dir(string $directory[, resource $context])
 * Description: Directory class with properties, handle and class and methods read, rewind and close
 * Source code: ext/standard/dir.c
 */

/*
 * Passing a non-existent directory as argument to dir() function
 * and checking to see if proper warning message is output.
 */
echo "*** Testing dir() : open a non-existent directory ***\n";

// create the temporary directory
$file_path = dirname(__FILE__);
$dir_path = $file_path."/dir_variation6";
@mkdir($dir_path);

// open existent directory
$d = dir($dir_path);
$d->close(); //close the dir

// remove directory and try to open the same(non-existent) directory again
rmdir($dir_path);
clearstatcache();

echo "-- opening previously removed directory --\n";
var_dump( dir($dir_path) );

// point to a non-existent directory
$non_existent_dir = $file_path."/non_existent_dir";
echo "-- opening non-existent directory --\n";
$d = dir($non_existent_dir);
var_dump( $d );

echo "Done";
?>
--EXPECTF--
*** Testing dir() : open a non-existent directory ***
-- opening previously removed directory --

Warning: dir(%s): failed to open dir: %s in %s on line %d
bool(false)
-- opening non-existent directory --

Warning: dir(%s): failed to open dir: %s in %s on line %d
bool(false)
Done
