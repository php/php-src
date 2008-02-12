--TEST--
Phar: tar-based phar, third-party tar with no stub, Phar->getStub()
--SKIPIF--
<?php if (!extension_loaded('phar')) die('skip'); ?>
<?php if (!extension_loaded("spl")) die("skip SPL not available"); ?>
--INI--
phar.readonly=1
--FILE--
<?php
include dirname(__FILE__) . '/files/tarmaker.php.inc';
$fname = dirname(__FILE__) . '/tar_004.phar.tar';
$alias = 'phar://' . $fname;

$tar = new tarmaker($fname, 'none');
$tar->init();
$tar->addFile('tar_004.php', '<?php var_dump(__FILE__);');
$tar->addFile('internal/file/here', "hi there!\n");
$tar->close();

$phar = new Phar($fname);
var_dump($phar->getStub());

?>
===DONE===
--CLEAN--
<?php
@unlink(dirname(__FILE__) . '/tar_004.phar');
?>
--EXPECTF--
string(0) ""
===DONE===
