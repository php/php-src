--TEST--
Test readfile() function: usage variation - include path
--FILE--
<?php
/* Prototype: int readfile ( string $filename [, bool $use_include_path [, resource $context]] );
   Description: Outputs a file
*/
/* test readfile() by providing an include path, second argument */

// include file.inc 
require("file.inc");

$file_path = dirname(__FILE__);
$dirname = "$file_path/readfile_variation3";

echo "*** Testing readfile(): checking second argument, include path ***\n";
// temp dir created
mkdir($dirname);
// temp file name used here
$filename = "$dirname/readfile_variation3.tmp";
// create file
$fp = fopen($filename, "w");
fill_file($fp, "text_with_new_line", 50);
fclose($fp);

// including $dirname in 'include_path'
ini_set('include_path',$dirname);
// 'include_path' set to true
$count = readfile("readfile_variation3.tmp", true);
echo "\n";
var_dump($count);
// use the context argument with include path
echo "*** Testing readfile(): checking second argument, include path with context specified ***\n";
$context = stream_context_create();
$count = readfile("readfile_variation3.tmp", true, $context);
echo "\n";
var_dump($count);

echo "Done\n";
?>
--CLEAN--
<?php
unlink(dirname(__FILE__)."/readfile_variation3/readfile_variation3.tmp");
rmdir(dirname(__FILE__)."/readfile_variation3");
?>
--EXPECT--
*** Testing readfile(): checking second argument, include path ***
line
line of text
line
line of text
line
line of t
int(50)
*** Testing readfile(): checking second argument, include path with context specified ***
line
line of text
line
line of text
line
line of t
int(50)
Done