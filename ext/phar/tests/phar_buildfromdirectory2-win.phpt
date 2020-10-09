--TEST--
Phar::buildFromDirectory() - non-directory passed as first parameter
--SKIPIF--
<?php
    if (!extension_loaded("phar")) die("skip");
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
--CLEAN--
<?php
unlink(__DIR__ . '/buildfromdirectory2.phar');
__HALT_COMPILER();
?>
--EXPECTF--
%s(24) "UnexpectedValueException"
RecursiveDirectoryIterator::__construct(1,1): %s (code: 2)
