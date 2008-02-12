--TEST--
Phar: tar-based phar, valid 1
--SKIPIF--
<?php if (!extension_loaded('phar')) die('skip'); ?>
<?php if (!extension_loaded("spl")) die("skip SPL not available"); ?>
--INI--
phar.readonly=0
--FILE--
<?php
include dirname(__FILE__) . '/files/tarmaker.php.inc';

$fname = dirname(__FILE__) . '/tar_003.phar.tar';
$alias = 'phar://' . $fname;

$tar = new tarmaker($fname, 'none');
$tar->init();
$tar->addFile('tar_003.phpt', $g = fopen(__FILE__, 'r'));
$tar->addFile('internal/file/here', "hi there!\n");
$tar->mkDir('internal/dir');
$tar->mkDir('dir');
$tar->close();

fclose($g);

echo file_get_contents($alias . '/internal/file/here');

$tar = opendir($alias . '/');

while (false !== ($v = readdir($tar))) {
	echo (is_file($alias . '/' . $v) ? "file\n" : "dir\n");
	echo $v . "\n";
}
closedir($tar);

/* ensure none of the dir tar files were freed */
echo "second round\n";
$tar = opendir($alias . '/');
while (false !== ($v = readdir($tar))) {
	echo (is_file($alias . '/' . $v) ? "file\n" : "dir\n");
	echo $v . "\n";
}
closedir($tar);

?>
===DONE===
--CLEAN--
<?php
@unlink(dirname(__FILE__) . '/tar_003.phar.tar');
?>
--EXPECT--
hi there!
dir
dir
dir
internal
file
tar_003.phpt
second round
dir
dir
dir
internal
file
tar_003.phpt
===DONE===