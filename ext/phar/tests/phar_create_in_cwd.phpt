--TEST--
Phar: attempt to create a Phar with relative path
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip"; ?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
chdir(dirname(__FILE__));
try {
	$p = new Phar('brandnewphar.phar');
	$p['file1.txt'] = 'hi';
	$p->commit();
	var_dump($p->getStub());
	$p->setStub("<?php
function __autoload(\$class)
{
    include 'phar://' . str_replace('_', '/', \$class);
}
Phar::mapPhar('brandnewphar.phar');
include 'phar://brandnewphar.phar/startup.php';
__HALT_COMPILER();
?>");
	var_dump($p->getStub());
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
?>
===DONE===
--CLEAN--
<?php 
unlink(dirname(__FILE__) . '/brandnewphar.phar');
?>
--EXPECT--
RecursiveDirectoryIterator::__construct(phar://brandnewphar.phar): failed to open dir: phar error: no directory in "phar://brandnewphar.phar", must have at least phar://brandnewphar.phar/ for root directory (always use full path to a new phar)
===DONE===