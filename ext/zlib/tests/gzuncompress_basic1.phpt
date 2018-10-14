--TEST--
Test gzuncompress() function : basic functionality
--SKIPIF--
<?php
if (!extension_loaded("zlib")) {
	print "skip - ZLIB extension not loaded";
}
?>
--FILE--
<?php
/* Prototype  : string gzuncompress(string data [, int length])
 * Description: Unzip a gzip-compressed string
 * Source code: ext/zlib/zlib.c
 * Alias to functions:
 */

include(dirname(__FILE__) . '/data.inc');

echo "*** Testing gzuncompress() : basic functionality ***\n";


// Initialise all required variables
$compressed = gzcompress($data);

echo "\n-- Basic decompress --\n";
var_dump(strcmp($data, gzuncompress($compressed)));


$length = 3547;
echo "\n-- Calling gzuncompress() with max length of $length --\n";
echo "Result length is ".  strlen(gzuncompress($compressed, $length)) .  "\n";

?>
===DONE===
--EXPECT--
*** Testing gzuncompress() : basic functionality ***

-- Basic decompress --
int(0)

-- Calling gzuncompress() with max length of 3547 --
Result length is 3547
===DONE===
