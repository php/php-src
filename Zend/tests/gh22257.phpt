--TEST--
GH-22257 (Type confusion / OOB read unserializing an Exception with a non-array trace)
--CREDITS--
Igor Sak-Sakovskiy (Positive Technologies)
--FILE--
<?php
/* A crafted, deliberately truncated payload makes the nested value of the typed
 * "array $trace" property fail to unserialize. On that failure path the slot used
 * to keep the half-built (non-array) value, and the partially-built Exception was
 * then exposed to getTraceAsString() through SplHeap's delayed __unserialize(),
 * type-confusing the object as a HashTable. The slot is now reset to the property
 * default, so the run completes without an out-of-bounds read. */
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

/* By-ref type violation: the slot is reset to the property default. */
class Test { public int $i; public array $a; }
try {
    var_dump(unserialize('O:4:"Test":2:{s:1:"i";N;s:1:"a";R:2;}'));
} catch (\Throwable $e) {
    printf("%s: %s\n", $e::class, $e->getMessage());
}
echo "OK\n";
?>
--EXPECTF--
Warning: unserialize(): Error at offset %d of %d bytes in %s on line %d
TypeError: Cannot assign null to property Test::$i of type int
OK
