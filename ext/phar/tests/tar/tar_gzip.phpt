--TEST--
Phar: tar-based phar, gzipped tar
--EXTENSIONS--
phar
zlib
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php
include __DIR__ . '/files/tarmaker.php.inc';
$fname = __DIR__ . '/tar_gzip.phar';
$pname = 'phar://' . $fname;
$fname2 = __DIR__ . '/tar_gzip.phar.tar';
$pname2 = 'phar://' . $fname2;

$a = new tarmaker($fname, 'zlib');
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
var_dump($b->isFileFormat(Phar::TAR));
var_dump($b->isCompressed() == Phar::GZ);
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/tar_gzip.phar');
@unlink(__DIR__ . '/tar_gzip.phar.tar');
?>
--EXPECTF--
string(9) "it worked"
string(%d) "phar://%star_gzip.phar/tar_004.php"
bool(true)
bool(true)
