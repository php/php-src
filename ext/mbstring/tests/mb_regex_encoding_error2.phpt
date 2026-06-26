--TEST--
Test mb_regex_encoding() function : error conditions - Pass an unknown encoding
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
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
--EXPECTF--
*** Testing mb_regex_encoding() : error conditions ***

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line 9
mb_regex_encoding(): Argument #1 ($encoding) must be a valid encoding, "unknown" given
