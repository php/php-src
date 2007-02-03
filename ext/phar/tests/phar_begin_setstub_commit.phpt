--TEST--
Phar::begin()/setStub()/commit()
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip"; ?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
$p = new Phar(dirname(__FILE__) . '/brandnewphar.phar', 0, 'brandnewphar.phar');
//var_dump($p->getStub());
var_dump($p->isFlushingToPhar());
$p->begin();
var_dump($p->isFlushingToPhar());
$p['a.php'] = '<?php var_dump("Hello");';
$p->setStub('<?php var_dump("First"); Phar::mapPhar("brandnewphar.phar"); __HALT_COMPILER(); ?>');
include 'phar://brandnewphar.phar/a.php';
var_dump($p->getStub());
$p['b.php'] = '<?php var_dump("World");';
$p->setStub('<?php var_dump("Second"); Phar::mapPhar("brandnewphar.phar"); __HALT_COMPILER(); ?>');
include 'phar://brandnewphar.phar/b.php';
var_dump($p->getStub());
$p->commit();
echo "===COMMIT===\n";
var_dump($p->isFlushingToPhar());
include 'phar://brandnewphar.phar/a.php';
include 'phar://brandnewphar.phar/b.php';
var_dump($p->getStub());
?>
===DONE===
--CLEAN--
<?php 
unlink(dirname(__FILE__) . '/brandnewphar.phar');
?>
--EXPECT--
bool(false)
bool(true)
string(5) "Hello"
string(82) "<?php var_dump("First"); Phar::mapPhar("brandnewphar.phar"); __HALT_COMPILER(); ?>"
string(5) "World"
string(83) "<?php var_dump("Second"); Phar::mapPhar("brandnewphar.phar"); __HALT_COMPILER(); ?>"
===COMMIT===
bool(false)
string(5) "Hello"
string(5) "World"
string(83) "<?php var_dump("Second"); Phar::mapPhar("brandnewphar.phar"); __HALT_COMPILER(); ?>"
===DONE===
