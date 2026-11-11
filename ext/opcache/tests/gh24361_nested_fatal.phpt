--TEST--
GH-24361 (bailouts from user code and existing GC protection are preserved)
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
--FILE--
<?php
class CompileErrorStream {
    public $context;

    public function stream_open($path, $mode, $options, &$opened_path) {
        eval('break 2;');
    }
}
stream_wrapper_register('compile-error', CompileErrorStream::class);

var_dump(gc_status()['protected']);
var_dump(@opcache_compile_file('compile-error://test'));
var_dump(gc_status()['protected']);
var_dump(@opcache_compile_file(__DIR__ . '/gh24361.inc'));
var_dump(gc_status()['protected']);
?>
--EXPECTF--
bool(false)

Fatal error: 'break' not in the 'loop' or 'switch' context in %s on line 1
bool(false)
bool(true)

Fatal error: 'break' not in the 'loop' or 'switch' context in %sgh24361.inc on line 2
bool(false)
bool(true)
