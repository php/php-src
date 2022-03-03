--TEST--
Phar::buildFromDirectory() - non-directory passed as first parameter
--EXTENSIONS--
phar
--SKIPIF--
<?php
    if (substr(PHP_OS, 0, 3) != "WIN") die("skip Windows only test");
?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
try {
    $phar = new Phar(__DIR__ . '/buildfromdirectory2.phar');
    $phar->buildFromDirectory(1);
} catch (Exception $e) {
    var_dump(get_class($e));
    echo $e->getMessage() . "\n";
}
?>
--EXPECTF--
%s(24) "UnexpectedValueException"
RecursiveDirectoryIterator::__construct(1): %s (code: 2)
