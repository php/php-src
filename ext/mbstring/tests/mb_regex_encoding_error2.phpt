--TEST--
Test mb_regex_encoding() function : error conditions - Pass an unknown encoding
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_regex_encoding') or die("skip mb_regex_encoding() is not available in this build");
?>
--FILE--
<?php
/*
 * Pass mb_regex_encoding an unknown type of encoding
 */

echo "*** Testing mb_regex_encoding() : error conditions ***\n";

try {
    var_dump(mb_regex_encoding('unknown'));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
*** Testing mb_regex_encoding() : error conditions ***
mb_regex_encoding(): Argument #1 ($encoding) must be a valid encoding, "unknown" given
