--TEST--
GH-17518 (offset overflow phar extractTo())
--EXTENSIONS--
phar
--INI--
phar.readonly=0
--FILE--
<?php
$fname = __DIR__.'/gh17518.phar.php';
$phar = new Phar($fname);
$phar['a'] = 'b';
try {
    $phar->extractTo(__DIR__ . '/gh17518', '');
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}
?>
--CLEAN--
<?php
@unlink(__DIR__.'/gh17518.phar.php');
?>
--EXPECT--
ValueError: Phar::extractTo(): Argument #2 ($files) must not be empty
