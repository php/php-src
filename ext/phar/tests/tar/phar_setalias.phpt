--TEST--
Phar::setAlias() tar-based
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php

$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.tar';
$fname2 = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '2.phar.tar';

$phar = new Phar($fname);
$phar->setStub('<?php echo "first stub\n"; __HALT_COMPILER(); ?>');
$phar->setAlias('hio');

$files = array();

$files['a'] = 'a';
$files['b'] = 'b';
$files['c'] = 'c';

foreach ($files as $n => $file) {
	$phar[$n] = $file;
}

$phar->stopBuffering();

echo $phar->getAlias() . "\n";
$phar->setAlias('test');
echo $phar->getAlias() . "\n";

copy($fname, $fname2);
$phar->setAlias('unused');
$a = new Phar($fname2);
echo $a->getAlias() . "\n";

?>
===DONE===
--CLEAN--
<?php 
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.tar');
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phartmp.tar');
__HALT_COMPILER();
?>
--EXPECT--
hio
test
test
===DONE===
