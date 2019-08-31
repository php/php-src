--TEST--
Test readfile() function: usage variation - stream_context
--FILE--
<?php
/* Prototype: int readfile ( string $filename [, bool $use_include_path [, resource $context]] );
   Description: Outputs a file
*/

/* test readfile() with third argument : context */

// include file.inc
require("file.inc");
$file_path = __DIR__;

/* Variation 1 : Check working of third argument of readfile() */

echo "*** Testing readfile(): checking third argument ***\n";
// creating a context
$context = stream_context_create();
// temp file name used here
$filename = "$file_path/readfile_variation1.tmp";

// create file
$fp = fopen($filename, "w");
fill_file($fp, "text_with_new_line", 50);
fclose($fp);
$count = readfile($filename, true, $context);
echo "\n";
var_dump($count);

echo "Done\n";
?>
--CLEAN--
<?php
unlink(__DIR__."/readfile_variation1.tmp");
?>
--EXPECT--
*** Testing readfile(): checking third argument ***
line
line of text
line
line of text
line
line of t
int(50)
Done
