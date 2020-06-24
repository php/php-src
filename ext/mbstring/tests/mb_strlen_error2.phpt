--TEST--
Test mb_strlen() function : error conditions - pass an unknown encoding
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_strlen') or die("skip mb_strlen() is not available in this build");
?>
--FILE--
<?php
/*
 * Test mb_strlen when passed an unknown encoding
 */

echo "*** Testing mb_strlen() : error ***\n";

$string = 'abcdef';

$encoding = 'unknown-encoding';

try {
    var_dump(mb_strlen($string, $encoding));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
*** Testing mb_strlen() : error ***
mb_strlen(): Argument #2 ($encoding) must be a valid encoding, "unknown-encoding" given
