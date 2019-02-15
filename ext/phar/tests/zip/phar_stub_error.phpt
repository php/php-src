--TEST--
Phar::setStub()/getStub() zip-based
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php

$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.zip';

$phar = new Phar($fname);
$phar->setStub($stub = '<?php echo "first stub\n"; __HALT_COMPILER(); ?>' . "\r\n");
$phar->setAlias('hio');
$phar['a'] = 'a';
$phar->stopBuffering();

var_dump($phar->getStub());
var_dump($phar->getStub() == $stub);

$newstub = '<?php echo "second stub\n"; _x_HALT_COMPILER(); ?>';
try
{
	$phar->setStub($newstub);
}
catch(exception $e)
{
	echo 'Exception: ' . $e->getMessage() . "\n";
}
var_dump($phar->getStub());
var_dump($phar->getStub() == $stub);
$phar->stopBuffering();
var_dump($phar->getStub());
var_dump($phar->getStub() == $stub);

?>
===DONE===
--CLEAN--
<?php
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.zip');
__HALT_COMPILER();
?>
--EXPECTF--
string(50) "<?php echo "first stub\n"; __HALT_COMPILER(); ?>
"
bool(true)
Exception: illegal stub for zip-based phar "%sphar_stub_error.phar.zip"
string(50) "<?php echo "first stub\n"; __HALT_COMPILER(); ?>
"
bool(true)
string(50) "<?php echo "first stub\n"; __HALT_COMPILER(); ?>
"
bool(true)
===DONE===
