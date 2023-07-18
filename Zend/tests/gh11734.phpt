--TEST--
GH-11734: Use-of-uninitialized-value when OOM on object allocation
--INI--
memory_limit=2M
--SKIPIF--
<?php
$zend_mm_enabled = getenv("USE_ZEND_ALLOC");
if ($zend_mm_enabled === "0") {
    die("skip Zend MM disabled");
}
?>
--FILE--
<?php
$objs = [];
while (true) {
    $objs[] = new SplPriorityQueue;
}
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted%S (tried to allocate %d bytes) in %s on line %d
