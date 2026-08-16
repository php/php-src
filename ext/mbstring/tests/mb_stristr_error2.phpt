--TEST--
Test mb_stristr() function : error conditions
--EXTENSIONS--
mbstring
--FILE--
<?php
echo "*** Testing mb_stristr() : error conditions ***\n";


echo "\n-- Testing mb_stristr() with unknown encoding --\n";
$haystack = 'Hello, world';
$needle = 'world';
$encoding = 'unknown-encoding';
$part = true;

try {
    var_dump( mb_stristr($haystack, $needle, $part, $encoding) );
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
*** Testing mb_stristr() : error conditions ***

-- Testing mb_stristr() with unknown encoding --
ValueError: mb_stristr(): Argument #4 ($encoding) must be a valid encoding, "unknown-encoding" given
