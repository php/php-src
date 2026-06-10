--TEST--
GH-22257 (Type confusion / OOB read in Exception::getTraceAsString() with a non-array trace)
--CREDITS--
Igor Sak-Sakovskiy (Positive Technologies)
--FILE--
<?php
/* A crafted, deliberately truncated payload leaves Exception::$trace holding a
 * non-array value (the typed-property check is skipped on the parse failure path).
 * The half-built object is then exposed to getTraceAsString() through SplHeap's
 * delayed __unserialize(), which used to type-confuse the object as a HashTable.
 * It must throw instead of reading out of bounds. */
$n = "\x00";
try {
    unserialize(
        'O:9:"Exception":1:{s:16:"' . $n . 'Exception' . $n . 'trace";' .
        'O:8:"stdClass":2:{s:1:"0";O:10:"SplMaxHeap":2:{i:0;a:0:{}i:1;a:2:{' .
        's:5:"flags";i:0;s:13:"heap_elements";a:2:{i:0;s:0:"";i:1;R:1;}}}z}}'
    );
} catch (\Throwable $e) {
    for (; $e; $e = $e->getPrevious()) {
        printf("%s: %s\n", $e::class, $e->getMessage());
    }
}
echo "OK\n";
?>
--EXPECTF--
Warning: unserialize(): Error at offset %d of %d bytes in %s on line %d
Exception: Invalid serialization data for SplMaxHeap object
Error: Exception trace is not an array
OK
