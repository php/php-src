--TEST--
GH-24361 (memory exhaustion while opening a script keeps GC protected)
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
memory_limit=16M
--SKIPIF--
<?php
if (getenv('USE_ZEND_ALLOC') === '0') {
    die('skip Zend MM disabled');
}
?>
--FILE--
<?php
class MemoryLimitStream {
    public $context;

    public function stream_open($path, $mode, $options, &$opened_path) {
        $memory = [];
        for ($i = 0; $i < 64; $i++) {
            $memory[] = str_repeat('x', 1024 * 1024);
        }
    }
}
stream_wrapper_register('memory-limit', MemoryLimitStream::class);
register_shutdown_function(static function () {
    var_dump(gc_status()['protected']);
});

var_dump(gc_status()['protected']);
opcache_compile_file('memory-limit://test');
echo "Unreachable\n";
?>
--EXPECTF--
bool(false)

Fatal error: Allowed memory size of %d bytes exhausted %s in %s on line %d
bool(true)
