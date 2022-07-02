--TEST--
Phar: tar-based phar, tar phar with stub, mapPhar()
--EXTENSIONS--
phar
--INI--
phar.readonly=0
phar.require_hash=0
opcache.validate_timestamps=1
--FILE--
<?php
include __DIR__ . '/files/tarmaker.php.inc';

$fname = __DIR__ . '/tar_004.phar.tar';
$alias = 'phar://' . $fname;

$tar = new tarmaker($fname, 'none');
$tar->init();
$tar->addFile('tar_004.php', '<?php var_dump(__FILE__);');
$tar->addFile('internal/file/here', "hi there!\n");
$tar->mkDir('internal/dir');
$tar->mkDir('dir');
$tar->addFile('.phar/stub.php', '<?php
Phar::mapPhar();
var_dump("it worked");
include "phar://" . __FILE__ . "/tar_004.php";
');
$tar->close();

include $fname;
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/tar_004.phar.tar');
?>
--EXPECTF--
string(9) "it worked"
string(%d) "phar://%star_004.phar.tar/tar_004.php"
