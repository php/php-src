--TEST--
Phar: Bug #79082: Files added to tar with Phar::buildFromIterator have all-access permissions
--SKIPIF--
<?php 
if (!extension_loaded("phar")) die("skip"); 
if (defined("PHP_WINDOWS_VERSION_MAJOR")) die("skip not for Windows")
?>
--FILE--
<?php
umask(022);
var_dump(decoct(umask()));
chmod(__DIR__ . '/test79082/test79082-testfile', 0644);
chmod(__DIR__ . '/test79082/test79082-testfile2', 0400);

foreach([Phar::TAR => 'tar', Phar::ZIP => 'zip'] as $mode => $ext) {
	clearstatcache();
	$phar = new PharData(__DIR__ . '/test79082.' . $ext, null, null, $mode);
	$phar->buildFromIterator(new \RecursiveDirectoryIterator(__DIR__ . '/test79082', \FilesystemIterator::SKIP_DOTS), __DIR__ . '/test79082');
	$phar->extractTo(__DIR__);
	var_dump(decoct(stat(__DIR__ . '/test79082-testfile')['mode']));
	var_dump(decoct(stat(__DIR__ . '/test79082-testfile2')['mode']));
	unlink(__DIR__ . '/test79082-testfile');
	unlink(__DIR__ . '/test79082-testfile2');
}
foreach([Phar::TAR => 'tar', Phar::ZIP => 'zip'] as $mode => $ext) {
	clearstatcache();
	$phar = new PharData(__DIR__ . '/test79082-d.' . $ext, null, null, $mode);
	$phar->buildFromDirectory(__DIR__ . '/test79082');
	$phar->extractTo(__DIR__);
	var_dump(decoct(stat(__DIR__ . '/test79082-testfile')['mode']));
	var_dump(decoct(stat(__DIR__ . '/test79082-testfile2')['mode']));
	unlink(__DIR__ . '/test79082-testfile');
	unlink(__DIR__ . '/test79082-testfile2');
}
?>
--CLEAN--
<?
unlink(__DIR__ . '/test79082.tar');
unlink(__DIR__ . '/test79082.zip');
unlink(__DIR__ . '/test79082-d.tar');
unlink(__DIR__ . '/test79082-d.zip');
?>
--EXPECT--
string(2) "22"
string(6) "100644"
string(6) "100400"
string(6) "100644"
string(6) "100400"
string(6) "100644"
string(6) "100400"
string(6) "100644"
string(6) "100400"
