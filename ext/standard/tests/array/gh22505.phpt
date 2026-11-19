--TEST--
GH-22505: Supplied range exceeds maximum array size by 0 elements
--SKIPIF--
<?php
if (PHP_INT_SIZE < 8) die("skip 64-bit only");
?>
--INI--
memory_limit=2M
--FILE--
<?php

try {
    range(0, 1073741824);
} catch (ValueError $e) {
    echo "Caught ValueError for 1073741824\n";
}

try {
    range(0.0, 1073741824.0, 1.0);
} catch (ValueError $e) {
    echo "Caught ValueError for float 1073741824.0\n";
}

try {
    range(0, 1073741823);
} catch (ValueError $e) {
    echo "Unexpected ValueError: " . $e->getMessage() . "\n";
}
?>
--EXPECTF--
Caught ValueError for 1073741824
Caught ValueError for float 1073741824.0

Fatal error: Allowed memory size of %d bytes exhausted %s
