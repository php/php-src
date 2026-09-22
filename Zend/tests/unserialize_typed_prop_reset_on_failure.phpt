--TEST--
unserialize() resets a typed property to its default on every failure path
--FILE--
<?php
/* A typed property whose value fails to unserialize is reset to its default, on
 * every failure path, so a partially built object can never expose a value that
 * violates the declared type. */

/* Failure while building the typed "array $trace", exposed through SplHeap's
 * delayed __unserialize(): the slot is reset, so getTraceAsString() no longer
 * reinterprets the object as a HashTable. */
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

/* By-ref type violation: the slot is reset to its default. */
class C { public array $a; }
try {
    var_dump(unserialize('O:1:"C":1:{s:1:"a";R:1;}'));
} catch (\Throwable $e) {
    printf("%s: %s\n", $e::class, $e->getMessage());
}
echo "OK\n";
?>
--EXPECTF--
Warning: unserialize(): Error at offset %d of %d bytes in %s on line %d
TypeError: %s
OK
