--TEST--
Phar::setAlias() error zip-based
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
$a->addFile('.phar/stub.php', '<?php echo "first stub\n"; __HALT_COMPILER(); ?>');

$files = array();
$files['a'] = 'a';
$files['b'] = 'b';
$files['c'] = 'c';
$files['.phar/alias.txt'] = 'hio';
foreach ($files as $n => $file) {
$a->addFile($n, $file);
}
$a->close();

$phar = new Phar($fname);
echo $phar->getAlias() . "\n";
$phar->setAlias('test');
echo $phar->getAlias() . "\n";
$phar = new Phar(dirname(__FILE__) . '/notphar.phar');
try {
	$phar->setAlias('test');
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
?>
===DONE===
--CLEAN--
<?php 
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php');
unlink(dirname(__FILE__) . '/notphar.phar');
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phartmp.php');
__HALT_COMPILER();
?>
--EXPECTF--
hio
test
alias "test" is already used for archive "%sphar_setalias2.phar.php" and cannot be used for other archives
===DONE===
