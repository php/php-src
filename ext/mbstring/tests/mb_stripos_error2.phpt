--TEST--
Test mb_stripos() function : error conditions - Pass unknown encoding
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_stripos') or die("skip mb_stripos() is not available in this build");
?>
--FILE--
<?php
/*
 * Pass an unknown encoding to mb_stripos() to test behaviour
 */

echo "*** Testing mb_stripos() : error conditions ***\n";
$haystack = 'Hello, world';
$needle = 'world';
$offset = 2;
$encoding = 'unknown-encoding';

try {
    var_dump( mb_stripos($haystack, $needle, $offset, $encoding) );
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
*** Testing mb_stripos() : error conditions ***
mb_stripos(): Argument #4 ($encoding) must be a valid encoding, "unknown-encoding" given
