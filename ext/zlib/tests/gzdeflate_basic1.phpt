--TEST--
Test gzdeflate() function : basic functionality
--EXTENSIONS--
zlib
--FILE--
<?php
/*
 * add a comment here to say what the test is supposed to do
 */

include(__DIR__ . '/data/data.inc');

echo "*** Testing gzdeflate() : basic functionality ***\n";

// Initialise all required variables

$smallstring = "A small string to compress\n";


// Calling gzdeflate() with all possible arguments

// Compressing a big string
for($i = -1; $i < 10; $i++) {
    echo "-- Compression level $i --\n";
    $output = gzdeflate($data, $i);
    var_dump(gzinflate($output) === $data);
}

// Compressing a smaller string
for($i = -1; $i < 10; $i++) {
    echo "-- Compression level $i --\n";
    $output = gzdeflate($smallstring, $i);
    var_dump(gzinflate($output) === $smallstring);
}

// Calling gzdeflate() with just mandatory arguments
echo "\n-- Testing with no specified compression level --\n";
$output = gzdeflate($smallstring);
var_dump(gzinflate($output) === $smallstring);

?>
--EXPECT--
*** Testing gzdeflate() : basic functionality ***
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
