--TEST--
Phar: tar-based phar, externally created tar with no stub, Phar->getStub()
--SKIPIF--
<?php if (!extension_loaded('phar')) die('skip'); ?>
<?php if (!extension_loaded("spl")) die("skip SPL not available"); ?>
--INI--
phar.readonly=1
--FILE--
<?php
include dirname(__FILE__) . '/tarmaker.php.inc';
$fname = dirname(__FILE__) . '/tar_004.phar';
$pname = 'phar://' . $fname;

$a = new tarmaker($fname, 'none');
$a->init();
$a->addFile('tar_004.php', '<?php var_dump(__FILE__);');
$a->addFile('internal/file/here', "hi there!\n");
$a->close();

$b = new Phar($fname);
var_dump($b->getStub());
?>
===DONE===
--CLEAN--
<?php
@unlink(dirname(__FILE__) . '/tar_004.phar');
?>
--EXPECTF--
string(0) ""
===DONE===