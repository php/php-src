--TEST--
Test chunk_split() function : usage variations - unexpected large '$end' string argument variation 1
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only");
if (getenv("USE_ZEND_ALLOC") === "0") die("skip ZMM is disabled");
?>
--FILE--
<?php
echo "*** Testing chunk_split() : unexpected large 'end' string argument variation 1 ***\n";

$a=str_repeat("B", 65535);
$b=1;
$c=str_repeat("B", 65535);

try {
    var_dump(chunk_split($a,$b,$c));
} catch (Throwable $t) {
    echo $t::class . ': ' . $t->getMessage() . "\n";
}
?>
--EXPECTF--
*** Testing chunk_split() : unexpected large 'end' string argument variation 1 ***
MemoryError: The resulting string is too large to fit in the configured memory limit
