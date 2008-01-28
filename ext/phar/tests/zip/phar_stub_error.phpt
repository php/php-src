--TEST--
Phar::setStub()/getStub() zip-based
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
include dirname(__FILE__) . '/tarmaker.php.inc';
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;
$a = new tarmaker($fname, 'none');
$a->init();
$a->addFile('.phar/stub.php', $stub = '<?php echo "first stub\n"; __HALT_COMPILER(); ?>');

$files = array();
$files['a'] = 'a';
$files['.phar/alias.txt'] = 'hio';
foreach ($files as $n => $file) {
$a->addFile($n, $file);
}
$a->close();

$phar = new Phar($fname);
var_dump($stub);
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
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php');
__HALT_COMPILER();
?>
--EXPECTF--
string(48) "<?php echo "first stub\n"; __HALT_COMPILER(); ?>"
string(48) "<?php echo "first stub\n"; __HALT_COMPILER(); ?>"
bool(true)
Exception: illegal stub for zip-based phar "%sphar_stub_error.phar.php"
string(48) "<?php echo "first stub\n"; __HALT_COMPILER(); ?>"
bool(true)
string(48) "<?php echo "first stub\n"; __HALT_COMPILER(); ?>"
bool(true)
===DONE===
