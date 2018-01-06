--TEST--
Bug #54289 Phar::extractTo() does not accept specific directories to be extracted
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly = 0
--FILE--
<?php
$inDir = __DIR__.DIRECTORY_SEPARATOR.'bug54289.source.d';
is_dir($inDir) or mkdir($inDir, 0700);
assert(is_dir($inDir));
assert(is_dir($inDir.DIRECTORY_SEPARATOR.'dirA') or mkdir($inDir.DIRECTORY_SEPARATOR.'dirA'));
assert(touch($inDir.DIRECTORY_SEPARATOR.'dirA'.DIRECTORY_SEPARATOR.'fileA'));

$tmpphar = __DIR__.DIRECTORY_SEPARATOR.'bug54289.phar';

$pharA = new Phar($tmpphar);
$pharA->buildFromDirectory($inDir);

$outDir = __DIR__.DIRECTORY_SEPARATOR.'bug54289.extract.d';
$pharB = new Phar($tmpphar);
$pharB->extractTo($outDir, 'dirA/', true);
assert(file_exists($outDir.DIRECTORY_SEPARATOR.'dirA'.DIRECTORY_SEPARATOR.'fileA'));
echo 'done'
?>
--EXPECTF--
done
--CLEAN--
$inDir = __DIR__.DIRECTORY_SEPARATOR.'bug54289.source.d';
unlink($inDir.DIRECTORY_SEPARATOR.'dirA'.DIRECTORY_SEPARATOR.'fileA');
rmdir($inDir.DIRECTORY_SEPARATOR.'dirA');
rmdir($inDir);
$outDir = __DIR__.DIRECTORY_SEPARATOR.'bug54289.extract.d';
unlink($outDir.DIRECTORY_SEPARATOR.'dirA'.DIRECTORY_SEPARATOR.'fileA');
rmdir($outDir.DIRECTORY_SEPARATOR.'dirA');
rmdir($outDir);
