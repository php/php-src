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
/*
 * Test basic function of gzencode
 */

include(__DIR__ . '/data.inc');

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

// Calling gzencode() with mandatory arguments
echo "\n-- Testing with no specified compression level --\n";
var_dump(bin2hex(gzencode($smallstring)));

echo "\n-- Testing gzencode with mode specified --\n";
var_dump(bin2hex(gzencode($smallstring, -1, FORCE_GZIP)));

?>
--EXPECTF--
*** Testing gzencode() : basic functionality ***
-- Compression level -1 --
string(32) "d9ede02415ce91d21e5a94274e2b9c42"
-- Compression level 0 --
string(32) "bbf32d5508e5f1f4e6d42790489dae15"
-- Compression level 1 --
string(32) "0bfaaa7a5a57f8fb533074fca6c85eeb"
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
string(32) "0f220a09e9895bcb3a1308d2bc99cfdf"
-- Compression level -1 --
string(32) "f77bd31e1e4dd11d12828fb661a08010"
-- Compression level 0 --
string(32) "9c5005db88490d6fe102ea2c233b2872"
-- Compression level 1 --
string(32) "d24ff7c4c20cef69b9c3abd603368db9"
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
string(32) "8849e9a1543c04b3f882b5ce20839ed2"

-- Testing with no specified compression level --
string(94) "1f8b08000000000000%c%c735428ce4dccc951282e29cacc4b5728c95748cecf2d284a2d2ee60200edc4e40b1b000000"

-- Testing gzencode with mode specified --
string(94) "1f8b08000000000000%c%c735428ce4dccc951282e29cacc4b5728c95748cecf2d284a2d2ee60200edc4e40b1b000000"
