--TEST--
Phar: tar-based phar, tar phar with stub, mapPhar()
--SKIPIF--
<?php
if (!extension_loaded("phar")) die("skip");
if (version_compare(PHP_VERSION, "6.0", "<")) die("skip Unicode support required");
if (!extension_loaded("spl")) die("skip SPL not available");
?>
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php
include dirname(__FILE__) . '/files/tarmaker.php.inc';

$fname = dirname(__FILE__) . '/tar_004U.phar.tar';
$alias = 'phar://' . $fname;

$tar = new tarmaker($fname, 'none');
$tar->init();
$tar->addFile('tar_004U.php', '<?php var_dump(__FILE__);');
$tar->addFile('internal/file/here', "hi there!\n");
$tar->mkDir('internal/dir');
$tar->mkDir('dir');
$tar->addFile('.phar/stub.php', '<?php
Phar::mapPhar();
var_dump("it worked");
include "phar://" . __FILE__ . "/tar_004U.php";
');
$tar->close();

include $fname;
?>
===DONE===
--CLEAN--
<?php
@unlink(dirname(__FILE__) . '/tar_004U.phar.tar');
?>
--EXPECTF--
unicode(9) "it worked"
unicode(%d) "phar://%star_004U.phar.tar/tar_004U.php"
===DONE===