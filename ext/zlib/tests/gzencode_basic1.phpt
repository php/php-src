--TEST--
Test gzencode() function : basic functionality 
--SKIPIF--
<?php 
if (!extension_loaded("zlib")) {
	print "skip - ZLIB extension not loaded"; 
}	 
?>
--FILE--
<?php
/* Prototype  : string gzencode  ( string $data  [, int $level  [, int $encoding_mode  ]] )
 * Description: Gzip-compress a string 
 * Source code: ext/zlib/zlib.c
 * Alias to functions: 
 */

/*
 * Test basic function of gzencode
 */

include(dirname(__FILE__) . '/data.inc');

echo "*** Testing gzencode() : basic functionality ***\n";

// Initialise all required variables

$smallstring = "A small string to compress\n";


// Calling gzencode() with various compression levels

// Compressing a big string
for($i = -1; $i < 10; $i++) {
    echo "-- Compression level $i --\n";
    $output = gzencode($data, $i);
    
    // Clear OS byte before encode 
	$output[9] = "\x00";
	
    var_dump(md5($output));
}

// Compressing a smaller string
for($i = -1; $i < 10; $i++) {
    echo "-- Compression level $i --\n";
    $output = gzencode($smallstring, $i);
    
    // Clear OS byte before encode 
	$output[9] = "\x00";
	
    var_dump(md5($output));
}

?>
===Done===
--EXPECTF--
*** Testing gzencode() : basic functionality ***
-- Compression level -1 --
string(32) "d9ede02415ce91d21e5a94274e2b9c42"
-- Compression level 0 --
string(32) "67aaf60426bb2cbd86d7fe530cb12306"
-- Compression level 1 --
string(32) "bce9c439cf767c1988ff4881b287d1ce"
-- Compression level 2 --
string(32) "7ddbfed63a76c42808722b66f1c133fc"
-- Compression level 3 --
string(32) "ca2b85d194dfa2a4e8a162b646c99265"
-- Compression level 4 --
string(32) "cfe28033eaf260bc33ddc04b53d3ba39"
-- Compression level 5 --
string(32) "ae357fada2b515422f8bea0aa3bcc48f"
-- Compression level 6 --
string(32) "d9ede02415ce91d21e5a94274e2b9c42"
-- Compression level 7 --
string(32) "d9ede02415ce91d21e5a94274e2b9c42"
-- Compression level 8 --
string(32) "d9ede02415ce91d21e5a94274e2b9c42"
-- Compression level 9 --
string(32) "d9ede02415ce91d21e5a94274e2b9c42"
-- Compression level -1 --
string(32) "f77bd31e1e4dd11d12828fb661a08010"
-- Compression level 0 --
string(32) "36220d650930849b67e8e0622f9bf270"
-- Compression level 1 --
string(32) "f77bd31e1e4dd11d12828fb661a08010"
-- Compression level 2 --
string(32) "f77bd31e1e4dd11d12828fb661a08010"
-- Compression level 3 --
string(32) "f77bd31e1e4dd11d12828fb661a08010"
-- Compression level 4 --
string(32) "f77bd31e1e4dd11d12828fb661a08010"
-- Compression level 5 --
string(32) "f77bd31e1e4dd11d12828fb661a08010"
-- Compression level 6 --
string(32) "f77bd31e1e4dd11d12828fb661a08010"
-- Compression level 7 --
string(32) "f77bd31e1e4dd11d12828fb661a08010"
-- Compression level 8 --
string(32) "f77bd31e1e4dd11d12828fb661a08010"
-- Compression level 9 --
string(32) "f77bd31e1e4dd11d12828fb661a08010"
===Done===