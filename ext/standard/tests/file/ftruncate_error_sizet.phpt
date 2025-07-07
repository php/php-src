--TEST--
ftruncate() on memory streams rejects sizes that exceed ZSTR_MAX_LEN
--SKIPIF--
<?php
if (PHP_SYS_SIZE >= PHP_INT_SIZE) {
    die("skip size_t is not narrower than zend_long on this platform");
}
?>
--FILE--
<?php

$sizes = [
    (1 << (PHP_SYS_SIZE * 8)) - 1,
    (1 << (PHP_SYS_SIZE * 8)) - 2,
    (1 << (PHP_SYS_SIZE * 8)),
    2 ** (PHP_SYS_SIZE * 8),
    2 ** (PHP_SYS_SIZE * 8) - 1,
    2 ** (PHP_SYS_SIZE * 8 - 1),
];

foreach ($sizes as $size) {
    try {
        $f = fopen('php://temp', 'w+');
        var_dump(ftruncate($f, $size));
    } catch (ValueError $e) {
        echo $e->getMessage(), "\n";
    }

    try {
        $f = fopen('php://memory', 'w+');
        var_dump(ftruncate($f, $size));
    } catch (ValueError $e) {
        echo $e->getMessage(), "\n";
    }
}

echo "done\n";
?>
--EXPECTF--
bool(false)
bool(false)
bool(false)
bool(false)
ftruncate(): Argument #2 ($size) must be less than or equal to %d
ftruncate(): Argument #2 ($size) must be less than or equal to %d
ftruncate(): Argument #2 ($size) must be less than or equal to %d
ftruncate(): Argument #2 ($size) must be less than or equal to %d
bool(false)
bool(false)

Fatal error: Allowed memory size of %d bytes exhausted at %s (tried to allocate %d bytes) in %s on line %d
