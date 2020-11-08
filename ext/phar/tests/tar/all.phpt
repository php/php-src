--TEST--
Phar: test that creation of tar-based phar generates valid tar with all bells/whistles
--SKIPIF--
<?php
if (!extension_loaded("phar")) die("skip");
?>
--INI--
phar.readonly=0
--FILE--
<?php

$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.tar.php';
$pname = 'phar://' . $fname;
$fname2 = __DIR__ . '/' . basename(__FILE__, '.php') . '.2.phar.tar.php';
$pname2 = 'phar://' . $fname2;

$phar = new Phar($fname);

$phar->setMetadata('hi there');
$phar['a'] = 'hi';
$phar['a']->setMetadata('a meta');
$phar['b'] = 'hi2';
$phar['c'] = 'hi3';
$phar['b']->chmod(0444);
$phar->setStub("<?php ok __HALT_COMPILER();");
$phar->setAlias("hime");
unset($phar);
copy($fname, $fname2);
Phar::unlinkArchive($fname);
var_dump(file_exists($fname), file_exists($pname . '/a'));

$phar = new Phar($fname2);
var_dump($phar['a']->getContent(), $phar['b']->getContent(), $phar['c']->getContent());
var_dump((string) decoct(fileperms($pname2 . '/b')));
var_dump($phar->getStub());
var_dump($phar->getAlias());
var_dump($phar->getMetadata());
var_dump($phar['a']->getMetadata());
?>
--CLEAN--
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.2.phar.tar.php'); ?>
--EXPECT--
bool(false)
bool(false)
string(2) "hi"
string(3) "hi2"
string(3) "hi3"
string(6) "100444"
string(32) "<?php ok __HALT_COMPILER(); ?>
"
string(4) "hime"
string(8) "hi there"
string(6) "a meta"
