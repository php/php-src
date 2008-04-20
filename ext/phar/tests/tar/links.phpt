--TEST--
Phar: tar with hard link and symbolic link
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--FILE--
<?php
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.tar';
$pname = 'phar://' . $fname;

include dirname(__FILE__) . '/files/corrupt_tarmaker.php.inc';
$a = new corrupt_tarmaker($fname, 'none');
$a->init();
$a->addFile('symlink', 'internal/file.txt', array(
                    'mode' => 0xA000 + 0644,
                    'uid' => 0,
                    'gid' => 0,
                    'size' => strlen('internal/file.txt'),
                    'mtime' => time(),
                ), 'symlink');
$a->addFile('hardlink', 'internal/file.txt', array(
                    'mode' => 0xA000 + 0644,
                    'uid' => 0,
                    'gid' => 0,
                    'size' => strlen('internal/file.txt'),
                    'mtime' => time(),
                ));
$a->addFile('internal/file.txt', 'hi there');
$a->close();

try {
	$p = new PharData($fname);
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
var_dump($p['symlink']->getContent());
var_dump($p['hardlink']->getContent());
var_dump($p['internal/file.txt']->getContent());
?>
===DONE===
--CLEAN--
<?php
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.tar');
?>
--EXPECTF--
string(17) "internal/file.txt"
string(17) "internal/file.txt"
string(8) "hi there"
===DONE===
