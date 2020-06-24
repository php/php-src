--TEST--
Test mb_strstr() function : error conditions
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_strstr') or die("skip mb_strstr() is not available in this build");
?>
--FILE--
<?php
echo "*** Testing mb_strstr() : error conditions ***\n";


echo "\n-- Testing mb_strstr() with unknown encoding --\n";
$haystack = 'Hello, world';
$needle = 'world';
$encoding = 'unknown-encoding';
$part = true;

try {
    var_dump( mb_strstr($haystack, $needle, $part, $encoding) );
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
*** Testing mb_strstr() : error conditions ***

-- Testing mb_strstr() with unknown encoding --
mb_strstr(): Argument #4 ($encoding) must be a valid encoding, "unknown-encoding" given
