--TEST--
Phar: tar-based phar, tar phar with stub, mapPhar()
--SKIPIF--
<?php if (!extension_loaded('phar')) die('skip'); ?>
<?php if (!extension_loaded("spl")) die("skip SPL not available"); ?>
--INI--
phar.readonly=0
--FILE--
<?php
include dirname(__FILE__) . '/tarmaker.php.inc';
$fname = dirname(__FILE__) . '/tar_004.phar';
$pname = 'phar://' . $fname;

$a = new tarmaker($fname, 'none');
$a->init();
$a->addFile('tar_004.php', '<?php var_dump(__FILE__);');
$a->addFile('internal/file/here', "hi there!\n");
$a->mkDir('internal/dir');
$a->mkDir('dir');
$a->addFile('.phar/stub.php', '<?php
Phar::mapPhar();
var_dump("it worked");
include "tar_004.php";
');
$a->close();

include $fname;
?>
===DONE===
--CLEAN--
<?php
@unlink(dirname(__FILE__) . '/tar_004.phar');
?>
--EXPECTF--
string(9) "it worked"
string(%d) "phar://%star_004.phar/tar_004.php"
===DONE===