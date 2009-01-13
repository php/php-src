--TEST--
Test gzcompress() function : basic functionality 
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

/*
 * add a comment here to say what the test is supposed to do
 */

include(dirname(__FILE__) . '/data.inc');

echo "*** Testing gzcompress() : basic functionality ***\n";

// Initialise all required variables

$smallstring = "A small string to compress\n";


// Calling gzcompress() with all possible arguments

// Compressing a big string
for($i = -1; $i < 10; $i++) {
    echo "-- Compression level $i --\n";
    $output = gzcompress(b"$data", $i);
    var_dump(md5($output));
	var_dump(strcmp(gzuncompress($output), $data));
}

// Compressing a smaller string
for($i = -1; $i < 10; $i++) {
    echo "-- Compression level $i --\n";
    $output = gzcompress(b"$smallstring", $i);
    var_dump(bin2hex($output));
	var_dump(strcmp(gzuncompress($output), $smallstring));
}

// Calling gzcompress() with mandatory arguments
echo "\n-- Testing with no specified compression level --\n";
var_dump( bin2hex(gzcompress(b"$smallstring") ));

?>
===Done===
--EXPECT--
*** Testing gzcompress() : basic functionality ***
-- Compression level -1 --
unicode(32) "764809aef15bb34cb73ad49ecb600d99"
int(0)
-- Compression level 0 --
unicode(32) "d0136b3fb5424142c0eb26dfec8f56fe"
int(0)
-- Compression level 1 --
unicode(32) "c2e070f4320d1f674965eaab95b53d9c"
int(0)
-- Compression level 2 --
unicode(32) "36922f486410d08209d0d0d21b26030e"
int(0)
-- Compression level 3 --
unicode(32) "a441a2f5169bb303cd45b860a5a9dbf9"
int(0)
-- Compression level 4 --
unicode(32) "d5b7451e9de2864beccc9de1fc55eb87"
int(0)
-- Compression level 5 --
unicode(32) "32ba4a01120449ec25508cabfad41f56"
int(0)
-- Compression level 6 --
unicode(32) "764809aef15bb34cb73ad49ecb600d99"
int(0)
-- Compression level 7 --
unicode(32) "e083e7e8d05471fed3c2182b9cd0d9eb"
int(0)
-- Compression level 8 --
unicode(32) "e083e7e8d05471fed3c2182b9cd0d9eb"
int(0)
-- Compression level 9 --
unicode(32) "e083e7e8d05471fed3c2182b9cd0d9eb"
int(0)
-- Compression level -1 --
unicode(70) "789c735428ce4dccc951282e29cacc4b5728c95748cecf2d284a2d2ee6020087a509cb"
int(0)
-- Compression level 0 --
unicode(76) "7801011b00e4ff4120736d616c6c20737472696e6720746f20636f6d70726573730a87a509cb"
int(0)
-- Compression level 1 --
unicode(70) "7801735428ce4dccc951282e29cacc4b5728c95748cecf2d284a2d2ee6020087a509cb"
int(0)
-- Compression level 2 --
unicode(70) "785e735428ce4dccc951282e29cacc4b5728c95748cecf2d284a2d2ee6020087a509cb"
int(0)
-- Compression level 3 --
unicode(70) "785e735428ce4dccc951282e29cacc4b5728c95748cecf2d284a2d2ee6020087a509cb"
int(0)
-- Compression level 4 --
unicode(70) "785e735428ce4dccc951282e29cacc4b5728c95748cecf2d284a2d2ee6020087a509cb"
int(0)
-- Compression level 5 --
unicode(70) "785e735428ce4dccc951282e29cacc4b5728c95748cecf2d284a2d2ee6020087a509cb"
int(0)
-- Compression level 6 --
unicode(70) "789c735428ce4dccc951282e29cacc4b5728c95748cecf2d284a2d2ee6020087a509cb"
int(0)
-- Compression level 7 --
unicode(70) "78da735428ce4dccc951282e29cacc4b5728c95748cecf2d284a2d2ee6020087a509cb"
int(0)
-- Compression level 8 --
unicode(70) "78da735428ce4dccc951282e29cacc4b5728c95748cecf2d284a2d2ee6020087a509cb"
int(0)
-- Compression level 9 --
unicode(70) "78da735428ce4dccc951282e29cacc4b5728c95748cecf2d284a2d2ee6020087a509cb"
int(0)

-- Testing with no specified compression level --
unicode(70) "789c735428ce4dccc951282e29cacc4b5728c95748cecf2d284a2d2ee6020087a509cb"
===Done===