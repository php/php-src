--TEST--
Phar: tar-based phar, valid 1
--SKIPIF--
<?php if (!extension_loaded('phar')) die('skip'); ?>
<?php if (!extension_loaded("spl")) die("skip SPL not available"); ?>
--INI--
phar.readonly=0
--FILE--
<?php
include dirname(__FILE__) . '/tarmaker.php.inc';
$fname = dirname(__FILE__) . '/tar_003.phar';
$pname = 'phar://' . $fname;

$a = new tarmaker($fname, 'none');
$a->init();
$a->addFile('tar_003.phpt', $g = fopen(__FILE__, 'r'));
$a->addFile('internal/file/here', "hi there!\n");
$a->mkDir('internal/dir');
$a->mkDir('dir');
$a->close();
fclose($g);

echo file_get_contents($pname . '/internal/file/here');
$a = opendir($pname . '/');
while (false !== ($v = readdir($a))) {
	echo (is_file($pname . '/' . $v) ? "file\n" : "dir\n");
	echo $v . "\n";
}
closedir($a);

?>
===DONE===
--CLEAN--
<?php
@unlink(dirname(__FILE__) . '/tar_003.phar');
?>
--EXPECT--
hi there!
dir
dir
dir
internal
file
tar_003.phpt
===DONE===