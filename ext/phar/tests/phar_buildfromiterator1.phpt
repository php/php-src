--TEST--
Phar::buildFromIterator() readonly
--EXTENSIONS--
phar
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
$phar = new Phar(__DIR__ . '/buildfromiterator1.phar');
try {
    ini_set('phar.readonly', 1);

    $phar->buildFromIterator(new ArrayIterator([]));
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECTF--
UnexpectedValueException: Cannot write out phar archive, phar is read-only
