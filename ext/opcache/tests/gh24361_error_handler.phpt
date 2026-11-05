--TEST--
GH-24361 (GC is restored before reporting the opcache_compile_file() warning)
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
--FILE--
<?php
set_error_handler(function ($severity, $message) {
    var_dump($severity === E_WARNING);
    var_dump(gc_status()['protected']);
    $node = new stdClass;
    $node->self = $node;
    unset($node);
    var_dump(gc_collect_cycles());
    throw new RuntimeException('compile warning');
});

try {
    opcache_compile_file(__DIR__ . '/gh24361.inc');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump(gc_status()['protected']);
?>
--EXPECTF--
Fatal error: 'break' not in the 'loop' or 'switch' context in %sgh24361.inc on line 2
bool(true)
bool(false)
int(1)
RuntimeException: compile warning
bool(false)
