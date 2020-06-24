--TEST--
Test mb_substr_count() function : error conditions - pass unknown encoding
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_substr_count') or die("skip mb_substr_count() is not available in this build");
?>
--FILE--
<?php
/*
 * Test behaviour of mb_substr_count() function when passed an unknown encoding
 */

echo "*** Testing mb_substr_count() : error conditions ***\n";

$haystack = 'Hello, World!';
$needle = 'Hello';
$encoding = 'unknown-encoding';

echo "\n-- Testing mb_substr_count() function with an unknown encoding --\n";

try {
    var_dump(mb_substr_count($haystack, $needle, $encoding));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
*** Testing mb_substr_count() : error conditions ***

-- Testing mb_substr_count() function with an unknown encoding --
mb_substr_count(): Argument #3 ($encoding) must be a valid encoding, "unknown-encoding" given
