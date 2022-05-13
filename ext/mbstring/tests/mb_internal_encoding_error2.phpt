--TEST--
Test mb_internal_encoding() function : error conditions - pass an unknown encoding
--EXTENSIONS--
mbstring
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
