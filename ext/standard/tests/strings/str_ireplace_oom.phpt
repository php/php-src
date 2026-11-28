--TEST--
str_ireplace() releases lowercased needle on OOM
--SKIPIF--
<?php
if (getenv('USE_ZEND_ALLOC') === '0') die('skip requires ZendMM');
?>
--INI--
memory_limit=4M
--FILE--
<?php
$base = memory_get_usage();
register_shutdown_function(function () use ($base) {
    var_dump(memory_get_usage() - $base < 100000);
});
function trigger(): void {
    $needle = str_repeat('A', 500000);
    str_ireplace($needle, str_repeat('b', 1000000), str_repeat('a', 1000000));
}
trigger();
?>
--EXPECTF--
Fatal error: Allowed memory size of 4194304 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
bool(true)
