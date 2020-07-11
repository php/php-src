--TEST--
Test mb_internal_encoding() function : error conditions - pass an unknown encoding
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_internal_encoding') or die("skip mb_internal_encoding() is not available in this build");
?>
--FILE--
<?php
/*
 * Pass mb_internal_encoding an unknown encoding
 */

echo "*** Testing mb_internal_encoding() : error conditions ***\n";

try {
    var_dump(mb_internal_encoding('unknown-encoding'));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
*** Testing mb_internal_encoding() : error conditions ***
mb_internal_encoding(): Argument #1 ($encoding) must be a valid encoding, "unknown-encoding" given
