--TEST--
Phar object: array access
--EXTENSIONS--
phar
--INI--
phar.require_hash=0
--FILE--
<?php

require_once 'files/phar_oo_test.inc';

class MyFile extends SplFileObject
{
    function __construct($what)
    {
        echo __METHOD__ . "($what)\n";
        parent::__construct($what);
    }
}

$phar = new Phar($fname);
try
{
    $phar->setFileClass('SplFileInfo');
}
catch (TypeError $e)
{
    echo $e->getMessage() . "\n";
}
$phar->setInfoClass('MyFile');

echo $phar['a.php']->getFilename() . "\n";
echo $phar['b/c.php']->getFilename() . "\n";
echo $phar['b.php']->getFilename() . "\n";

?>
--CLEAN--
<?php
unlink(__DIR__ . '/files/phar_oo_006.phar.php');
__halt_compiler();
?>
--EXPECTF--
SplFileInfo::setFileClass(): Argument #1 ($class) must be a class name derived from SplFileObject, SplFileInfo given
MyFile::__construct(phar://%s/a.php)
a.php
MyFile::__construct(phar://%s/b/c.php)
c.php
MyFile::__construct(phar://%s/b.php)
b.php
