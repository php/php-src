--TEST--
output buffering - fatalism
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows' && ini_get('opcache.jit') && ini_get('opcache.jit_buffer_size') && !PHP_DEBUG) {
    die('xfail crashes on Windows with JITted release builds');
}
?>
--FILE--
<?php
function obh($s)
{
    return ob_get_flush();
}
ob_start("obh");
echo "foo\n";
?>
--EXPECTF--
Fatal error: ob_get_flush(): Cannot use output buffering in output buffering display handlers in %sob_011.php on line %d
