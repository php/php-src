--TEST--
stream_copy_to_stream() $maxLength overflow on narrow size_t
--SKIPIF--
<?php
if (PHP_SYS_SIZE >= PHP_INT_SIZE) {
    die("skip size_t is not narrower than zend_long on this platform");
}
?>
--FILE--
<?php
$src = fopen('php://memory', 'r+');
$dst = fopen('php://memory', 'w+');
fwrite($src, 'testdata');
rewind($src);
try {
    stream_copy_to_stream($src, $dst, PHP_INT_MAX);
    echo "unexpected success\n";
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
fclose($src);
fclose($dst);
?>
--EXPECTF--
stream_copy_to_stream(): Argument #3 ($length) must be less than or equal to %d
