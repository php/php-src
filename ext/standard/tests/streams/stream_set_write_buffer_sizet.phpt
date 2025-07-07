--TEST--
stream_set_write_buffer() $size overflow on narrow size_t
--SKIPIF--
<?php
if (PHP_SYS_SIZE >= PHP_INT_SIZE) {
    die("skip size_t is not narrower than zend_long on this platform");
}
?>
--FILE--
<?php
$s = fopen('php://memory', 'w+');
try {
    stream_set_write_buffer($s, PHP_INT_MAX);
    echo "unexpected success\n";
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
fclose($s);
?>
--EXPECTF--
stream_set_write_buffer(): Argument #2 ($size) must be less than or equal to %d
