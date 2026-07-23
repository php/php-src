--TEST--
Phar::buildFromDirectory() - non-directory passed as first parameter
--EXTENSIONS--
phar
--SKIPIF--
<?php
    if (substr(PHP_OS, 0, 3) == "WIN") die("skip not for Windows");
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
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECTF--
UnexpectedValueException: RecursiveDirectoryIterator::__construct(1): Failed to open directory: No such file or directory
