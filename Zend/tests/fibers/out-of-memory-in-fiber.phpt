--TEST--
Out of Memory in a fiber
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
    $buffer = '';
    while (true) {
        $buffer .= str_repeat('.', 1 << 10);
    }
});

$fiber->start();

?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted%s(tried to allocate %d bytes) in %sout-of-memory-in-fiber.php on line %d
