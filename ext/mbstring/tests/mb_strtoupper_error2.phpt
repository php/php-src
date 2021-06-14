--TEST--
Test mb_strtoupper() function : error conditions - pass an unknown encoding
--EXTENSIONS--
mbstring
--FILE--
<?php
/*
 * Pass an unknown encoding as $encoding argument to check behaviour of mbstrtoupper()
 */

echo "*** Testing mb_strtoupper() : error conditions ***\n";

$sourcestring = 'hello, world';
$encoding = 'unknown-encoding';

try {
    var_dump( mb_strtoupper($sourcestring, $encoding) );
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
*** Testing mb_strtoupper() : error conditions ***
mb_strtoupper(): Argument #2 ($encoding) must be a valid encoding, "unknown-encoding" given
