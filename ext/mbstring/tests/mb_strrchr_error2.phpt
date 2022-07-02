--TEST--
Test mb_strrchr() function : error conditions
--EXTENSIONS--
mbstring
--FILE--
<?php
echo "*** Testing mb_strrchr() : error conditions ***\n";


echo "\n-- Testing mb_strrchr() with unknown encoding --\n";
$haystack = 'Hello, world';
$needle = 'world';
$encoding = 'unknown-encoding';
$part = true;

try {
    var_dump( mb_strrchr($haystack, $needle, $part, $encoding) );
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
*** Testing mb_strrchr() : error conditions ***

-- Testing mb_strrchr() with unknown encoding --
mb_strrchr(): Argument #4 ($encoding) must be a valid encoding, "unknown-encoding" given
