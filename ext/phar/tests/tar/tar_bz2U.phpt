--TEST--
Phar: tar-based phar, bzipped tar
--SKIPIF--
<?php
if (!extension_loaded("phar")) die("skip");
if (version_compare(PHP_VERSION, "6.0", "!=")) die("skip Unicode support required");
if (!extension_loaded("spl")) die("skip SPL not available");
if (!extension_loaded("bz2")) die("skip bz2 not available");
?>
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php
include dirname(__FILE__) . '/files/tarmaker.php.inc';

$fname = dirname(__FILE__) . '/tar_bz2U.phar';
$alias = 'phar://' . $fname;
$fname2 = dirname(__FILE__) . '/tar_bz2U.phar.tar';
$alias2 = 'phar://' . $fname2;

$tar = new tarmaker($fname, 'bz2');
$tar->init();
$tar->addFile('tar_004.php', '<?php var_dump(__FILE__);');
$tar->addFile('internal/file/here', "hi there!\n");
$tar->mkDir('internal/dir');
$tar->mkDir('dir');
$tar->addFile('.phar/stub.php', '<?php
var_dump(__FILE__);
var_dump(substr(__FILE__, 0, 4) != "phar");
Phar::mapPhar();
var_dump("it worked");
include "phar://" . __FILE__ . "/tar_004.php";
__HALT_COMPILER();
');
$tar->close();

include $alias;

$phar = new Phar($fname);
$phar['test'] = 'hi';

copy($fname, $fname2);

$phar2 = new Phar($fname2);
var_dump($phar2->isFileFormat(Phar::TAR));
var_dump($phar2->isCompressed() == Phar::BZ2);

?>
===DONE===
--CLEAN--
<?php
@unlink(dirname(__FILE__) . '/tar_bz2U.phar');
@unlink(dirname(__FILE__) . '/tar_bz2U.phar.tar');
?>
--EXPECTF--
unicode(%d) "%star_bz2U.phar"
bool(true)
unicode(9) "it worked"
unicode(%d) "phar://%star_bz2U.phar/tar_004.php"
bool(true)
bool(true)
===DONE===
