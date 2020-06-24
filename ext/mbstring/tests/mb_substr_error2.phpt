--TEST--
Test mb_substr() function : error conditions - Pass an unknown encoding
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_substr') or die("skip mb_substr() is not available in this build");
?>
--FILE--
<?php
/*
 * Pass an unknown encoding to mb_substr() to test behaviour
 */

echo "*** Testing mb_substr() : error conditions ***\n";

$str = 'Hello, world';
$start = 1;
$length = 5;
$encoding = 'unknown-encoding';

try {
    var_dump( mb_substr($str, $start, $length, $encoding));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
*** Testing mb_substr() : error conditions ***
mb_substr(): Argument #4 ($encoding) must be a valid encoding, "unknown-encoding" given
