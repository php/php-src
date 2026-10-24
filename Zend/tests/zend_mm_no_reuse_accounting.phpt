--TEST--
ZendMM no-reuse mode keeps retired huge mappings accounted for
--ENV--
USE_ZEND_ALLOC=1
ZEND_MM_NO_REUSE=1
--FILE--
<?php
putenv("ZEND_MM_NO_REUSE=0");
$initial = memory_get_usage(true);
$buffer = str_repeat("x", 3 * 1024 * 1024);
$allocated = memory_get_usage();
$committed = memory_get_usage(true);
unset($buffer);
$released = memory_get_usage();
$retained = memory_get_usage(true);
$collected = gc_mem_caches();

var_dump($committed - $initial >= 3 * 1024 * 1024);
var_dump($allocated - $released >= 3 * 1024 * 1024);
var_dump($retained === $committed);
var_dump($collected === 0);
var_dump(memory_get_usage(true) === $retained);

register_shutdown_function(function () use ($retained) {
    var_dump(memory_get_usage(true) >= $retained);
    $buffer = str_repeat("y", 3 * 1024 * 1024);
    unset($buffer);
    var_dump(memory_get_usage(true) > $retained);
});
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
