--TEST--
fgets() $length overflow on narrow size_t
--SKIPIF--
<?php
if (PHP_SYS_SIZE >= PHP_INT_SIZE) {
    die("skip size_t is not narrower than zend_long on this platform");
}
?>
--FILE--
<?php
$f = fopen(__FILE__, 'r');
try {
    fgets($f, PHP_INT_MAX);
    echo "unexpected success\n";
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
fclose($f);
?>
--EXPECTF--
fgets(): Argument #2 ($length) must be less than or equal to %d
