--TEST--
GH-24361 (GC recovery does not rely on the last reported error)
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
ignore_repeated_errors=1
ignore_repeated_source=1
display_errors=0
log_errors=0
--FILE--
<?php
class RepeatedErrorStream {
    public $context;

    public function stream_open($path, $mode, $options, &$opened_path) {
        trigger_error("'break' not in the 'loop' or 'switch' context", E_USER_ERROR);
    }
}
stream_wrapper_register('repeated-error', RepeatedErrorStream::class);
set_error_handler(static fn() => true, E_WARNING | E_DEPRECATED);
for ($i = 0; $i < 2; $i++) {
    var_dump(opcache_compile_file(__DIR__ . '/gh24361.inc'));
    var_dump(gc_status()['protected']);
}
var_dump(opcache_compile_file('repeated-error://test'));
var_dump(gc_status()['protected']);
var_dump(error_get_last()['type'] === E_COMPILE_ERROR);
?>
--EXPECT--
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
