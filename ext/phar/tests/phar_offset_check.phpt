--TEST--
Phar: disallow stub and alias setting via offset*() methods
--EXTENSIONS--
phar
--INI--
phar.readonly=0
phar.require_hash=1
--FILE--
<?php

$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://'.$fname;

$phar = new Phar($fname);
$phar->setDefaultStub();
$phar->setAlias('susan');
$phar['a.txt'] = "first file\n";
$phar['b.txt'] = "second file\n";

try {
    $phar->offsetGet('.phar/stub.php');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}

try {
    $phar->offsetGet('.phar/alias.txt');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}

try {
    $phar->offsetSet('.phar/stub.php', '<?php __HALT_COMPILER(); ?>');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}

var_dump(strlen($phar->getStub()));

try {
    $phar->offsetUnset('.phar/stub.php');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}

var_dump(strlen($phar->getStub()));

try {
    $phar->offsetSet('.phar/alias.txt', 'dolly');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}

var_dump($phar->getAlias());

try {
    $phar->offsetUnset('.phar/alias.txt');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}

var_dump($phar->getAlias());

?>
--CLEAN--
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECT--
BadMethodCallException: Entry .phar/stub.php does not exist
BadMethodCallException: Entry .phar/alias.txt does not exist
ValueError: Phar::offsetSet(): Argument #1 ($localName) must not be ".phar/stub.php", use Phar::setStub() instead
int(6661)
int(6661)
ValueError: Phar::offsetSet(): Argument #1 ($localName) must not be ".phar/alias.txt", use Phar::setAlias() instead
string(5) "susan"
string(5) "susan"
