--TEST--
Test gzdeflate() function : basic functionality
--SKIPIF--
<?php
if (!extension_loaded("zlib")) {
	print "skip - ZLIB extension not loaded";
}
?>
--FILE--
<?php
/* Prototype  : string gzdeflate(string data [, int level, [int encoding]])
 * Description: Gzip-compress a string
 * Source code: ext/zlib/zlib.c
 * Alias to functions:
 */

/*
 * add a comment here to say what the test is supposed to do
 */

include(__DIR__ . '/data.inc');

echo "*** Testing gzdeflate() : basic functionality ***\n";

// Initialise all required variables

$smallstring = "A small string to compress\n";


// Calling gzdeflate() with all possible arguments

// Compressing a big string
for($i = -1; $i < 10; $i++) {
    echo "-- Compression level $i --\n";
    $output = gzdeflate($data, $i);
    var_dump(md5($output));
	var_dump(strcmp(gzinflate($output), $data));
}

// Compressing a smaller string
for($i = -1; $i < 10; $i++) {
    echo "-- Compression level $i --\n";
    $output = gzdeflate($smallstring, $i);
    var_dump(bin2hex($output));
	var_dump(strcmp(gzinflate($output), $smallstring));
}

// Calling gzdeflate() with just mandatory arguments
echo "\n-- Testing with no specified compression level --\n";
var_dump( bin2hex(gzdeflate($smallstring) ));

?>
===Done===
--EXPECT--
*** Testing gzdeflate() : basic functionality ***
-- Compression level -1 --
string(32) "078554fe65e06f6ff01eab51cfc7ae9b"
int(0)
-- Compression level 0 --
string(32) "a71e54d2499aff9e48643cb1c260b60c"
int(0)
-- Compression level 1 --
string(32) "05e80f4dc0d422e1f333cbed555d381f"
int(0)
-- Compression level 2 --
string(32) "0fb33656e4ed0750f977df83246fce7a"
int(0)
-- Compression level 3 --
string(32) "bc6e9c1dccc3e951e006315ee669ee08"
int(0)
-- Compression level 4 --
string(32) "a61727d7a28c634470eb6e97a4a81b24"
int(0)
-- Compression level 5 --
string(32) "a2a1a14b7542c82e8943200d093d5f27"
int(0)
-- Compression level 6 --
string(32) "078554fe65e06f6ff01eab51cfc7ae9b"
int(0)
-- Compression level 7 --
string(32) "078554fe65e06f6ff01eab51cfc7ae9b"
int(0)
-- Compression level 8 --
string(32) "078554fe65e06f6ff01eab51cfc7ae9b"
int(0)
-- Compression level 9 --
string(32) "078554fe65e06f6ff01eab51cfc7ae9b"
int(0)
-- Compression level -1 --
string(58) "735428ce4dccc951282e29cacc4b5728c95748cecf2d284a2d2ee60200"
int(0)
-- Compression level 0 --
string(64) "011b00e4ff4120736d616c6c20737472696e6720746f20636f6d70726573730a"
int(0)
-- Compression level 1 --
string(58) "735428ce4dccc951282e29cacc4b5728c95748cecf2d284a2d2ee60200"
int(0)
-- Compression level 2 --
string(58) "735428ce4dccc951282e29cacc4b5728c95748cecf2d284a2d2ee60200"
int(0)
-- Compression level 3 --
string(58) "735428ce4dccc951282e29cacc4b5728c95748cecf2d284a2d2ee60200"
int(0)
-- Compression level 4 --
string(58) "735428ce4dccc951282e29cacc4b5728c95748cecf2d284a2d2ee60200"
int(0)
-- Compression level 5 --
string(58) "735428ce4dccc951282e29cacc4b5728c95748cecf2d284a2d2ee60200"
int(0)
-- Compression level 6 --
string(58) "735428ce4dccc951282e29cacc4b5728c95748cecf2d284a2d2ee60200"
int(0)
-- Compression level 7 --
string(58) "735428ce4dccc951282e29cacc4b5728c95748cecf2d284a2d2ee60200"
int(0)
-- Compression level 8 --
string(58) "735428ce4dccc951282e29cacc4b5728c95748cecf2d284a2d2ee60200"
int(0)
-- Compression level 9 --
string(58) "735428ce4dccc951282e29cacc4b5728c95748cecf2d284a2d2ee60200"
int(0)

-- Testing with no specified compression level --
string(58) "735428ce4dccc951282e29cacc4b5728c95748cecf2d284a2d2ee60200"
===Done===
