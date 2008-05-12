--TEST--
Phar::setAlias() error
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;
$file = '<?php echo "first stub\n"; __HALT_COMPILER(); ?>';

$files = array();
$files['a'] = 'a';
$files['b'] = 'b';
$files['c'] = 'c';

include 'files/phar_test.inc';

$phar = new Phar($fname);
echo $phar->getAlias() . "\n";
$phar->setAlias('test');
echo $phar->getAlias() . "\n";
$b = $phar;
$phar = new Phar(dirname(__FILE__) . '/notphar.phar');
try {
	$phar->setAlias('test');
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
try {
	$b = new Phar(dirname(__FILE__) . '/nope.phar', 0, 'test');
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
?>
===DONE===
--CLEAN--
<?php 
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php');
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phartmp.php');
unlink(dirname(__FILE__) . '/notphar.phar');
__HALT_COMPILER();
?>
--EXPECTF--
hio
test
alias "test" is already used for archive "%sphar_setalias2.phar.php" and cannot be used for other archives
alias "test" is already used for archive "%sphar_setalias2.phar.php" cannot be overloaded with "%snope.phar"
===DONE===
