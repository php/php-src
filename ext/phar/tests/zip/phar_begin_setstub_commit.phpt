--TEST--
Phar::startBuffering()/setStub()/stopBuffering() zip-based
--SKIPIF--
<?php
if (!extension_loaded("phar")) die("skip");
?>
--INI--
phar.readonly=0
--FILE--
<?php
$p = new Phar(__DIR__ . '/phar_begin_setstub_commit.phar.zip', 0, 'phar_begin_setstub_commit.phar');
var_dump($p->isFileFormat(Phar::ZIP));
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

// add portion to test setting stub from resource
file_put_contents(__DIR__ . '/myfakestub.php', '<?php var_dump("First resource"); Phar::mapPhar("phar_begin_setstub_commit.phar"); __HALT_COMPILER(); ?>');
$a = fopen(__DIR__ . '/myfakestub.php', 'rb');
$p->setStub($a);
var_dump($p->getStub());
$c = strlen('<?php var_dump("First resource"); Phar::mapPhar("phar_begin_setstub_commit.phar"); __HALT_COMPILER(); ?>');
file_put_contents(__DIR__ . '/myfakestub.php', '<?php var_dump("First resource"); Phar::mapPhar("phar_begin_setstub_commit.phar"); __HALT_COMPILER(); ?>' . 'extra stuff');
fseek($a, 0);
$p->setStub($a, $c);
var_dump($p->getStub());
fclose($a);
?>
===DONE===
--CLEAN--
<?php
unlink(__DIR__ . '/phar_begin_setstub_commit.phar.zip');
unlink(__DIR__ . '/myfakestub.php');
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
string(%d) "<?php var_dump("First resource"); Phar::mapPhar("phar_begin_setstub_commit.phar"); __HALT_COMPILER(); ?>
"
string(%d) "<?php var_dump("First resource"); Phar::mapPhar("phar_begin_setstub_commit.phar"); __HALT_COMPILER(); ?>
"
===DONE===
