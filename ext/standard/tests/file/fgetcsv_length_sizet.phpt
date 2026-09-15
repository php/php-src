--TEST--
fgetcsv() $length overflow on narrow size_t
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
    fgetcsv($f, 2 ** (PHP_SYS_SIZE * 8), ',', '"', '\\');
    echo "unexpected success\n";
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
fclose($f);
?>
--EXPECTF--
fgetcsv(): Argument #2 ($length) must be between 0 and %d
