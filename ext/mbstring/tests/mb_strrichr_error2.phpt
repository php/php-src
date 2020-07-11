--TEST--
Test mb_strrichr() function : error conditions
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_strrichr') or die("skip mb_strrichr() is not available in this build");
?>
--FILE--
<?php
echo "*** Testing mb_strrichr() : error conditions ***\n";


echo "\n-- Testing mb_strrichr() with unknown encoding --\n";
$haystack = 'Hello, world';
$needle = 'world';
$encoding = 'unknown-encoding';
$part = true;

try {
    var_dump( mb_strrichr($haystack, $needle, $part, $encoding) );
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
*** Testing mb_strrichr() : error conditions ***

-- Testing mb_strrichr() with unknown encoding --
mb_strrichr(): Argument #4 ($encoding) must be a valid encoding, "unknown-encoding" given
