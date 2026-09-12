--TEST--
stream_get_line() $length overflow on narrow size_t
--SKIPIF--
<?php
if (PHP_SYS_SIZE >= PHP_INT_SIZE) {
    die("skip size_t is not narrower than zend_long on this platform");
}
?>
--FILE--
<?php
$s = fopen('php://memory', 'r+');
fwrite($s, "line\n");
rewind($s);
try {
    stream_get_line($s, PHP_INT_MAX);
    echo "unexpected success\n";
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
fclose($s);
?>
--EXPECTF--
stream_get_line(): Argument #2 ($length) must be less than or equal to %d
