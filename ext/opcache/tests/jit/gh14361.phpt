--TEST--
GH-14361 (Deep recursion in zend_cfg.c causes segfault instead of error)
--SKIPIF--
<?php
if (!function_exists('zend_test_zend_call_stack_get')) die("skip zend_test_zend_call_stack_get() is not available");
if (PHP_ZTS !== 0) die("skip gives additional warning on ZTS");
?>
--EXTENSIONS--
zend_test
opcache
--INI--
error_reporting=E_COMPILE_ERROR
zend.max_allowed_stack_size=128K
opcache.enable=1
opcache.enable_cli=1
opcache.jit=tracing
opcache.jit_buffer_size=64M
opcache.optimization_level=0x000000a0
--FILE--
<?php
$tmp = fopen(__DIR__."/gh14361_tmp.php", "w");
fwrite($tmp, '<?php ');
for ($i = 0; $i < 70000; $i++) {
    fwrite($tmp, 'if (@((0) == (0)) !== (true)) { $f++; }');
}
fclose($tmp);

include __DIR__."/gh14361_tmp.php";
?>
--CLEAN--
<?php
@unlink(__DIR__."/gh14361_tmp.php");
?>
--EXPECTF--
Fatal error: Maximum call stack size of %d bytes (zend.max_allowed_stack_size - zend.reserved_stack_size) reached during compilation. Try reducing function size in %s on line %d
