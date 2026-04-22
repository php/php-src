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
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}
try {
    $p['.phar/alias.txt'] = 'hi';
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}
$p = new Phar($fname2);
try {
    $p['.phar/stub.php'] = 'hi';
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}
try {
    $p['.phar/alias.txt'] = 'hi';
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}

?>
--EXPECT--
ValueError: Phar::offsetSet(): Argument #1 ($localName) must not be ".phar/stub.php", use Phar::setStub() instead
ValueError: Phar::offsetSet(): Argument #1 ($localName) must not be ".phar/alias.txt", use Phar::setAlias() instead
ValueError: Phar::offsetSet(): Argument #1 ($localName) must not be ".phar/stub.php", use Phar::setStub() instead
ValueError: Phar::offsetSet(): Argument #1 ($localName) must not be ".phar/alias.txt", use Phar::setAlias() instead
