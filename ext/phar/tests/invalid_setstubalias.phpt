--TEST--
Phar: invalid set alias or stub via array access
--EXTENSIONS--
phar
--INI--
phar.readonly=0
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.tar';
$fname2 = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.zip';

$p = new Phar($fname);
try {
    $p['.phar/stub.php'] = 'hi';
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $p['.phar/alias.txt'] = 'hi';
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
$p = new Phar($fname2);
try {
    $p['.phar/stub.php'] = 'hi';
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $p['.phar/alias.txt'] = 'hi';
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECTF--
BadMethodCallException: Cannot set stub ".phar/stub.php" directly in phar "%sinvalid_setstubalias.phar.tar", use setStub
BadMethodCallException: Cannot set alias ".phar/alias.txt" directly in phar "%sinvalid_setstubalias.phar.tar", use setAlias
BadMethodCallException: Cannot set stub ".phar/stub.php" directly in phar "%sinvalid_setstubalias.phar.zip", use setStub
BadMethodCallException: Cannot set alias ".phar/alias.txt" directly in phar "%sinvalid_setstubalias.phar.zip", use setAlias
