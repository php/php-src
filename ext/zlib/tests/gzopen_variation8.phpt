--TEST--
Test gzopen() function : variation: opening a plain file
--SKIPIF--
<?php 
if (!extension_loaded("zlib")) {
	print "skip - ZLIB extension not loaded"; 
}	 
?>
--FILE--
<?php
/* Prototype  : resource gzopen(string filename, string mode [, int use_include_path])
 * Description: Open a .gz-file and return a .gz-file pointer 
 * Source code: ext/zlib/zlib.c
 * Alias to functions: 
 */

echo "*** Testing gzopen() : variation ***\n";

$data = <<<EOT
Here is some plain
text to be read
and displayed.
EOT;

$file = "gzopen_variation8.tmp";
$h = fopen($file, 'w');
fwrite($h, $data);
fclose($h);

$h = gzopen($file, 'r');
gzpassthru($h);
gzclose($h);
echo "\n";
unlink($file);
?>
===DONE===
--EXPECT--
*** Testing gzopen() : variation ***
Here is some plain
text to be read
and displayed.
===DONE===
