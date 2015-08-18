--TEST--
Phar::startBuffering()/setStub()/stopBuffering()
--SKIPIF--
<?php
if (!extension_loaded("phar")) die("skip");
if (version_compare(PHP_VERSION, "6.0", "==")) die("skip pre-unicode version of PHP required");
?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
$p = new Phar(dirname(__FILE__) . '/brandnewphar.phar', 0, 'brandnewphar.phar');
//var_dump($p->getStub());
var_dump($p->isBuffering());
$p->startBuffering();
var_dump($p->isBuffering());
$p['a.php'] = '<?php var_dump("Hello");';
$p->setStub('<?php var_dump("First"); Phar::mapPhar("brandnewphar.phar"); __HALT_COMPILER(); ?>');
include 'phar://brandnewphar.phar/a.php';
var_dump($p->getStub());
$p['b.php'] = '<?php var_dump("World");';
$p->setStub('<?php var_dump("Second"); Phar::mapPhar("brandnewphar.phar"); __HALT_COMPILER();');
include 'phar://brandnewphar.phar/b.php';
var_dump($p->getStub());
$p->stopBuffering();
echo "===COMMIT===\n";
var_dump($p->isBuffering());
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
string(84) "<?php var_dump("First"); Phar::mapPhar("brandnewphar.phar"); __HALT_COMPILER(); ?>
"
string(5) "World"
string(85) "<?php var_dump("Second"); Phar::mapPhar("brandnewphar.phar"); __HALT_COMPILER(); ?>
"
===COMMIT===
bool(false)
string(5) "Hello"
string(5) "World"
string(85) "<?php var_dump("Second"); Phar::mapPhar("brandnewphar.phar"); __HALT_COMPILER(); ?>
"
===DONE===
