--TEST--
Test chunk_split() function : usage variations - unexpected large number of chunks
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") === "0") die("skip ZMM is disabled");
?>
--FILE--
<?php
$chunk_length = 1;

echo "*** Testing chunk_split() : unexpected large 'end' string argument variation 2 ***\n";

echo "Body generation\n";
$body = str_repeat("Hello", 10000000);

echo "Using chunk_split()\n";
try {
    var_dump(chunk_split($body, $chunk_length));
} catch (MemoryError $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
*** Testing chunk_split() : unexpected large 'end' string argument variation 2 ***
Body generation
Using chunk_split()
The resulting string is too large to fit in the configured memory limit
