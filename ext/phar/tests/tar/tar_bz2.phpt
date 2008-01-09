--TEST--
Phar: tar-based phar, bzipped tar
--SKIPIF--
<?php if (!extension_loaded('phar')) die('skip'); ?>
<?php if (!extension_loaded("spl")) die("skip SPL not available"); ?>
<?php if (!extension_loaded("bz2")) die("skip bz2 not available"); ?>
--INI--
phar.readonly=0
--FILE--
<?php
include dirname(__FILE__) . '/tarmaker.php.inc';
$fname = dirname(__FILE__) . '/tar_bz2.phar';
$pname = 'phar://' . $fname;
$fname2 = dirname(__FILE__) . '/tar_bz2.phar.tar';
$pname2 = 'phar://' . $fname2;

$a = new tarmaker($fname, 'bz2');
$a->init();
$a->addFile('tar_004.php', '<?php var_dump(__FILE__);');
$a->addFile('internal/file/here', "hi there!\n");
$a->mkDir('internal/dir');
$a->mkDir('dir');
$a->addFile('.phar/stub.php', '<?php
Phar::mapPhar();
var_dump("it worked");
include "phar://" . __FILE__ . "/tar_004.php";
');
$a->close();

include $fname;

$a = new Phar($fname);
$a['test'] = 'hi';
copy($fname, $fname2);
$b = new Phar($fname2);
var_dump($b->isTar());
var_dump($b->isCompressed() == Phar::BZ2);
?>
===DONE===
--CLEAN--
<?php
@unlink(dirname(__FILE__) . '/tar_bz2.phar');
@unlink(dirname(__FILE__) . '/tar_bz2.phar.tar');
?>
--EXPECTF--
string(9) "it worked"
string(%d) "phar://%star_bz2.phar/tar_004.php"
bool(true)
bool(true)
===DONE===