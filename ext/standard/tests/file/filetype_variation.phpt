--TEST--
Test filetype() function: Variations
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip no link()/symlink() on Windows');
}
if (!function_exists("posix_mkfifo")) {
	die("skip no posix_mkfifo()");
}
?>
--FILE--
<?php
/*
Prototype: string filetype ( string $filename );
Description: Returns the type of the file. Possible values are fifo, char,
             dir, block, link, file, and unknown. 
*/

include "file.inc";

echo "*** Testing filetype() with various types ***\n";

create_files( dirname(__FILE__), 2);

echo "-- Checking with files --\n";
print( filetype( dirname(__FILE__)."/file1.tmp") )."\n";
print( filetype( dirname(__FILE__)."/file2.tmp") )."\n";
clearstatcache();

echo "-- Checking with links: hardlink --\n";
link( dirname(__FILE__)."/file1.tmp", dirname(__FILE__)."/link1.tmp");
print( filetype( dirname(__FILE__)."/link1.tmp" ) )."\n";

echo "-- Checking with links: symlink --\n";
symlink( dirname(__FILE__)."/file2.tmp", dirname(__FILE__)."/link2.tmp");
print( filetype( dirname(__FILE__)."/link2.tmp") )."\n";

delete_files( dirname(__FILE__), 2, "link");

delete_files( dirname(__FILE__), 2, "file");

$file_path = dirname(__FILE__);
echo "-- Checking with directory --\n";
mkdir("$file_path/temp");
print( filetype("$file_path/temp") )."\n";
rmdir( "$file_path/temp" );

echo "-- Checking with fifo --\n";
posix_mkfifo( dirname(__FILE__)."/file3.tmp", 0755);
print( filetype( dirname(__FILE__)."/file3.tmp") )."\n";
delete_files( dirname(__FILE__), 1, "file", 3);

/* Checking with block in file */
/* To test this PEAR package should be installed */

echo "\n*** Done ***\n";
?>
--EXPECTF--
*** Testing filetype() with various types ***
-- Checking with files --
file
file
-- Checking with links: hardlink --
file
-- Checking with links: symlink --
link
-- Checking with directory --
dir
-- Checking with fifo --
fifo

*** Done ***
