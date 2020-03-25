--TEST--
Phar object: basics
--SKIPIF--
<?php
if (!extension_loaded("phar")) die("skip");
?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php

require_once 'files/phar_oo_test.inc';

$phar = new Phar($fname);
var_dump($phar->getVersion());
var_dump(count($phar));

class MyPhar extends Phar
{
    function __construct()
    {
    }
}

try
{
    $phar = new MyPhar();
    var_dump($phar->getVersion());
}
catch (LogicException $e)
{
    var_dump($e->getMessage());
}
try {
    $phar = new Phar('test.phar');
    $phar->__construct('oops');
} catch (LogicException $e)
{
    var_dump($e->getMessage());
}

?>
--CLEAN--
<?php
unlink(__DIR__ . '/files/phar_oo_001.phar.php');
__halt_compiler();
?>
--EXPECT--
string(5) "1.0.0"
int(5)
string(50) "Cannot call method on an uninitialized Phar object"
string(29) "Cannot call constructor twice"
