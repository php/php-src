--TEST--
Phar::startBuffering()/setStub()/stopBuffering() tar-based
--EXTENSIONS--
phar
--INI--
phar.readonly=0
--FILE--
<?php
$p = new Phar(__DIR__ . '/phar_begin_setstub_commit.phar.tar', 0, 'phar_begin_setstub_commit.phar');
var_dump($p->isFileFormat(Phar::TAR));
//var_dump($p->getStub());
var_dump($p->isBuffering());
$p->startBuffering();
var_dump($p->isBuffering());
$p['a.php'] = '<?php var_dump("Hello");';
$p->setStub('<?php var_dump("First"); Phar::mapPhar("phar_begin_setstub_commit.phar"); __HALT_COMPILER(); ?>');
include 'phar://phar_begin_setstub_commit.phar/a.php';
var_dump($p->getStub());
$p['b.php'] = '<?php var_dump("World");';
$p->setStub('<?php var_dump("Second"); Phar::mapPhar("phar_begin_setstub_commit.phar"); __HALT_COMPILER();');
include 'phar://phar_begin_setstub_commit.phar/b.php';
var_dump($p->getStub());
$p->stopBuffering();
echo "===COMMIT===\n";
var_dump($p->isBuffering());
include 'phar://phar_begin_setstub_commit.phar/a.php';
include 'phar://phar_begin_setstub_commit.phar/b.php';
var_dump($p->getStub());
?>
--CLEAN--
<?php
unlink(__DIR__ . '/phar_begin_setstub_commit.phar.tar');
?>
--EXPECTF--
bool(true)
bool(false)
bool(true)
string(5) "Hello"
string(%d) "<?php var_dump("First"); Phar::mapPhar("phar_begin_setstub_commit.phar"); __HALT_COMPILER(); ?>
"
string(5) "World"
string(%d) "<?php var_dump("Second"); Phar::mapPhar("phar_begin_setstub_commit.phar"); __HALT_COMPILER(); ?>
"
===COMMIT===
bool(false)
string(5) "Hello"
string(5) "World"
string(%d) "<?php var_dump("Second"); Phar::mapPhar("phar_begin_setstub_commit.phar"); __HALT_COMPILER(); ?>
"
