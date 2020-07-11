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
$files['b'] = 'b';
$files['c'] = 'c';

include 'files/phar_test.inc';

$phar = new Phar($fname);
var_dump($stub);
var_dump($phar->getStub());
var_dump($phar->getStub() == $stub);

$stub = '<?php echo "second stub\n"; __HALT_COMPILER(); ?>';
$sexp = $stub . "\r\n";

$phar->setStub($stub);
var_dump($phar->getStub());
var_dump($phar->getStub() == $stub);
var_dump($phar->getStub() == $sexp);
$phar->stopBuffering();
var_dump($phar->getStub());
var_dump($phar->getStub() == $stub);
var_dump($phar->getStub() == $sexp);

$phar = new Phar($fname);
var_dump($phar->getStub() == $stub);
var_dump($phar->getStub() == $sexp);

?>
--CLEAN--
<?php
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.php');
__HALT_COMPILER();
?>
--EXPECT--
string(48) "<?php echo "first stub\n"; __HALT_COMPILER(); ?>"
string(48) "<?php echo "first stub\n"; __HALT_COMPILER(); ?>"
bool(true)
string(51) "<?php echo "second stub\n"; __HALT_COMPILER(); ?>
"
bool(false)
bool(true)
string(51) "<?php echo "second stub\n"; __HALT_COMPILER(); ?>
"
bool(false)
bool(true)
bool(false)
bool(true)
