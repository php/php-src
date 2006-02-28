--TEST--
Phar object: basics
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip"; ?>
--FILE--
<?php

require_once 'phar_oo_test.inc';

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
catch (BadMethodCallException $e)
{
	var_dump($e->getMessage());
}

?>
===DONE===
--CLEAN--
<?php 
unlink(dirname(__FILE__) . '/phar_oo_test.phar.php');
__halt_compiler();
?>
--EXPECT--
string(5) "0.8.0"
int(5)
string(49) "Cannot call method on an uninitialzed Phar object"
===DONE===
