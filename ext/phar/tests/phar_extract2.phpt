--TEST--
Phar: Phar::extractTo() - .phar safety
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=0
--FILE--
<?php

$fname = dirname(__FILE__) . '/tempmanifest2.phar.php';
$pname = 'phar://' . $fname;

$phar = new Phar($fname);
$phar->setDefaultStub();
$phar->setAlias('fred');
$phar['file1.txt'] = 'hi';
$phar['file2.txt'] = 'hi2';
$phar['subdir/ectory/file.txt'] = 'hi3';
$phar->mount($pname . '/mount', __FILE__);
$phar->addEmptyDir('one/level');

$phar->extractTo(dirname(__FILE__) . '/extract', 'mount');
$phar->extractTo(dirname(__FILE__) . '/extract');
$out = array();

foreach (new RecursiveIteratorIterator(new RecursiveDirectoryIterator(dirname(__FILE__) . '/extract', 0x00003000), RecursiveIteratorIterator::CHILD_FIRST) as $path => $file) {
	$extracted[] = $path;
}

sort($extracted);

foreach ($extracted as $out) {
	echo "$out\n";
}

?>
===DONE===
--CLEAN--
<?php
@unlink(dirname(__FILE__) . '/tempmanifest2.phar.php');
$dir = dirname(__FILE__) . '/extract/';
@unlink($dir . 'file1.txt');
@unlink($dir . 'file2.txt');
@unlink($dir . 'subdir/ectory/file.txt');
@rmdir($dir . 'subdir/ectory');
@rmdir($dir . 'subdir');
@rmdir($dir . 'one/level');
@rmdir($dir . 'one');
@rmdir($dir);
$dir = dirname(__FILE__) . '/extract1/';
@rmdir($dir);
?>
--EXPECTF--
%sextract%cfile1.txt
%sextract%cfile2.txt
%sextract%cone
%sextract%csubdir
%sextract%csubdir%cectory
%sextract%csubdir%cectory%cfile.txt
===DONE===
