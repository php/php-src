--TEST--
Test gzcompress() function : variation
--SKIPIF--
<?php
if (!extension_loaded("zlib")) {
	print "skip - ZLIB extension not loaded";
}
?>
--FILE--
<?php
/* Prototype  : string gzcompress(string data [, int level, [int encoding]])
 * Description: Gzip-compress a string
 * Source code: ext/zlib/zlib.c
 * Alias to functions:
 */

include(dirname(__FILE__) . '/data.inc');

echo "*** Testing gzcompress() : variation ***\n";

echo "\n-- Testing multiple compression --\n";
$output = gzcompress($data);
var_dump( md5($output));
var_dump(md5(gzcompress($output)));

?>
===Done===
--EXPECT--
*** Testing gzcompress() : variation ***

-- Testing multiple compression --
string(32) "764809aef15bb34cb73ad49ecb600d99"
string(32) "eba942bc2061f23ea8688cc5101872a4"
===Done===
