--TEST--
Test Csv\collection_to_file(): a failure while flushing buffered data on close is reported
--EXTENSIONS--
csv
--SKIPIF--
<?php
if (!extension_loaded('zlib')) die('skip zlib extension not available');
if (!file_exists('/dev/full')) die('skip /dev/full not available');
?>
--FILE--
<?php
/* The compressed stream buffers the row and only fails when the deflate buffer
 * is flushed to /dev/full during close. */
try {
    Csv\collection_to_file('compress.zlib:///dev/full', [['a', 'b']]);
    echo 'no error', \PHP_EOL;
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}
?>
--EXPECT--
Error: Failed to finish writing to "compress.zlib:///dev/full"
