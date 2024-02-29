--TEST--
Test gzencode() function : basic functionality
--EXTENSIONS--
zlib
--SKIPIF--
<?php if (getenv('TRAVIS')) die('skip Currently fails on Travis'); ?>
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
    var_dump(strcmp(gzdecode($output), $data)===0);
}

// Compressing a smaller string
for($i = -1; $i < 10; $i++) {
    echo "-- Compression level $i --\n";
    $output = gzencode($smallstring, $i);
    var_dump(strcmp(gzdecode($output), $smallstring)===0);
}

// Calling gzencode() with mandatory arguments
echo "\n-- Testing with no specified compression level --\n";
$output = gzencode($smallstring);
var_dump(strcmp(gzdecode($output), $smallstring)===0);

echo "\n-- Testing gzencode with mode specified --\n";
$outupt = gzencode($smallstring, -1, FORCE_GZIP);
var_dump(strcmp(gzdecode($output), $smallstring)===0);
?>
--EXPECTF--
*** Testing gzencode() : basic functionality ***
-- Compression level -1 --
bool(true)
-- Compression level 0 --
bool(true)
-- Compression level 1 --
bool(true)
-- Compression level 2 --
bool(true)
-- Compression level 3 --
bool(true)
-- Compression level 4 --
bool(true)
-- Compression level 5 --
bool(true)
-- Compression level 6 --
bool(true)
-- Compression level 7 --
bool(true)
-- Compression level 8 --
bool(true)
-- Compression level 9 --
bool(true)
-- Compression level -1 --
bool(true)
-- Compression level 0 --
bool(true)
-- Compression level 1 --
bool(true)
-- Compression level 2 --
bool(true)
-- Compression level 3 --
bool(true)
-- Compression level 4 --
bool(true)
-- Compression level 5 --
bool(true)
-- Compression level 6 --
bool(true)
-- Compression level 7 --
bool(true)
-- Compression level 8 --
bool(true)
-- Compression level 9 --
bool(true)

-- Testing with no specified compression level --
bool(true)

-- Testing gzencode with mode specified --
bool(true)
