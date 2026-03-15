--TEST--
Out of Memory in a nested fiber
--INI--
memory_limit=2M
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") === "0") {
    die("skip Zend MM disabled");
}
?>
--FILE--
<?php

$fiber = new Fiber(function (): void {
    $fiber = new Fiber(function (): void {
        $buffer = '';
        while (true) {
            $buffer .= str_repeat('.', 1 << 10);
        }
    });

    $fiber->start();
});

$fiber->start();

?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted by %d bytes%S. Allocated %d bytes and need to allocate %d bytes to satisfy a request for %d bytes in %s on line %d
