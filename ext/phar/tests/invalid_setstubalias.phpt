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
    echo $e->getMessage() . "\n";
}
try {
    $p['.phar/alias.txt'] = 'hi';
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}
$p = new Phar($fname2);
try {
    $p['.phar/stub.php'] = 'hi';
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}
try {
    $p['.phar/alias.txt'] = 'hi';
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECTF--
Cannot set stub ".phar/stub.php" directly in phar "%sinvalid_setstubalias.phar.tar", use setStub
Cannot set alias ".phar/alias.txt" directly in phar "%sinvalid_setstubalias.phar.tar", use setAlias
Cannot set stub ".phar/stub.php" directly in phar "%sinvalid_setstubalias.phar.zip", use setStub
Cannot set alias ".phar/alias.txt" directly in phar "%sinvalid_setstubalias.phar.zip", use setAlias
