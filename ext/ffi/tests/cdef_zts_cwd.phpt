--TEST--
FFI::cdef() loads a relative library from the current directory after chdir() on Linux ZTS
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Linux' || !PHP_ZTS) {
    die('skip requires Linux ZTS');
}
try {
    $libc = FFI::cdef('int unshare(int flags);');
} catch (FFI\Exception) {
    die('skip unshare() is unavailable');
}
if ($libc->unshare(0x200 /* CLONE_FS */) !== 0) {
    die('skip requires per-thread filesystem state');
}
require dirname(__DIR__, 2) . '/dl_test/tests/skip.inc';
?>
--FILE--
<?php
$dir = realpath(ini_get('extension_dir'));
$decl = 'struct _zend_module_entry *get_module(void);';
chdir($dir);
$relative = FFI::cdef($decl, './dl_test.so');
$absolute = FFI::cdef($decl, $dir . '/dl_test.so');
var_dump($relative->get_module() == $absolute->get_module());
?>
--EXPECT--
bool(true)
