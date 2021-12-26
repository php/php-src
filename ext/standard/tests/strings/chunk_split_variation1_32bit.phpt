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
var_dump(chunk_split($a,$b,$c));
?>
--EXPECTF--
*** Testing chunk_split() : unexpected large 'end' string argument variation 1 ***

Fatal error: %rAllowed memory size of %d bytes exhausted%s\(tried to allocate %d bytes\)|Possible integer overflow in memory allocation \(4294901777 \+ 2097152\)%r in %s on line %d
