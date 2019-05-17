--TEST--
Test readfile() function: usage variations - links
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip.. only on Linux');
}
?>
--FILE--
<?php
/* Prototype: int readfile ( string $filename [, bool $use_include_path [, resource $context]] );
   Description: Outputs a file
*/

/* Variation 2 : Create file
                 Create soft/hard link to it
                 Read link using readfile()
                 Delete file and its link
*/

// include file.inc
require("file.inc");

$file_path = __DIR__;

// temp file used here
$filename = "$file_path/readfile_variation2.tmp";

// create temp file and insert data into it
$fp = fopen($filename, "w");
fill_file($fp, "text_with_new_line", 50);
fclose($fp);

// temp link name used
$linkname = "$file_path/readfile_variation2_link.tmp";

/* Checking readfile() operation on soft link */
echo "*** Testing readfile() on soft link ***\n";

// create soft link to $filename
var_dump( symlink($filename, $linkname) );
// readfile() on soft link
$count = readfile($linkname); // with default args
echo "\n";
var_dump($count);
// delete link
unlink($linkname);

/* Checking readfile() operation on hard link */
echo "\n*** Testing readfile() on hard link ***\n";
// create hard link to $filename
var_dump( link($filename, $linkname) );
// readfile() on hard link
$count = readfile($linkname); // default args
echo "\n";
var_dump($count);
// delete link
unlink($linkname);

echo "Done\n";
?>
--CLEAN--
<?php
$file_path = __DIR__;
unlink("$file_path/readfile_variation2.tmp");
?>
--EXPECT--
*** Testing readfile() on soft link ***
bool(true)
line
line of text
line
line of text
line
line of t
int(50)

*** Testing readfile() on hard link ***
bool(true)
line
line of text
line
line of text
line
line of t
int(50)
Done
