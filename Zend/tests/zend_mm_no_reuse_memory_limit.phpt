--TEST--
ZendMM no-reuse mode charges retired allocations against the memory limit
--ENV--
USE_ZEND_ALLOC=1
ZEND_MM_NO_REUSE=1
--INI--
memory_limit=16M
--FILE--
<?php
for ($i = 0; $i < 100; $i++) {
    $buffer = str_repeat("x", 1024 * 1024);
    unset($buffer);
    gc_mem_caches();
}
echo "Unexpected reuse\n";
?>
--EXPECTF--
Fatal error: Allowed memory size of 16777216 bytes exhausted %s
