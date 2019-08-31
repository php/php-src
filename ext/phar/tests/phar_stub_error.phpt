--TEST--
Phar::setStub()/getStub()
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;
$stub = '<?php echo "first stub\n"; __HALT_COMPILER(); ?>';
$file = $stub;

$files = array();
$files['a'] = 'a';

include 'files/phar_test.inc';

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
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.php');
__HALT_COMPILER();
?>
--EXPECTF--
string(48) "<?php echo "first stub\n"; __HALT_COMPILER(); ?>"
string(48) "<?php echo "first stub\n"; __HALT_COMPILER(); ?>"
bool(true)
Exception: illegal stub for phar "%sphar_stub_error.phar.php"
string(48) "<?php echo "first stub\n"; __HALT_COMPILER(); ?>"
bool(true)
string(48) "<?php echo "first stub\n"; __HALT_COMPILER(); ?>"
bool(true)
===DONE===
