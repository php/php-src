--TEST--
Phar::buildFromDirectory() - readonly
--EXTENSIONS--
phar
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
$phar = new Phar(__DIR__ . '/buildfromdirectory1.phar');
try {
    ini_set('phar.readonly', 1);
    $phar->buildFromDirectory(1);
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECTF--
UnexpectedValueException: Cannot write to archive - write operations restricted by INI setting
