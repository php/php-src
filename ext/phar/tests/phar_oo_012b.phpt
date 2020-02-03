--TEST--
Phar object: unset file
--SKIPIF--
<?php if (!extension_loaded('phar')) die('skip'); ?>
--INI--
phar.readonly=1
phar.require_hash=0
--FILE--
<?php

try
{
    $pharconfig = 0;

    require_once 'files/phar_oo_test.inc';

    $phar = new Phar($fname);
    $phar->setInfoClass('SplFileObject');

    $phar['f.php'] = 'hi';
    var_dump(isset($phar['f.php']));
    echo $phar['f.php'];
    echo "\n";
    unset($phar['f.php']);
    var_dump(isset($phar['f.php']));
}
catch (BadMethodCallException $e)
{
    echo "Exception: " . $e->getMessage() . "\n";
}

?>
--CLEAN--
<?php
unlink(__DIR__ . '/files/phar_oo_012b.phar.php');
__halt_compiler();
?>
--EXPECT--
Exception: Write operations disabled by the php.ini setting phar.readonly
