--TEST--
Bug #54289 Phar::extractTo() does not accept specific directories to be extracted
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly = 0
--FILE--
<?php
// creates a directory structure
$inDir = __DIR__.DIRECTORY_SEPARATOR.'bug54289.source.d';
is_dir($inDir) or mkdir($inDir, 0700);
assert(is_dir($inDir));
assert(is_dir($inDir.DIRECTORY_SEPARATOR.'dirA') or mkdir($inDir.DIRECTORY_SEPARATOR.'dirA'));
assert(touch($inDir.DIRECTORY_SEPARATOR.'dirA'.DIRECTORY_SEPARATOR.'fileA1'));
assert(touch($inDir.DIRECTORY_SEPARATOR.'dirA'.DIRECTORY_SEPARATOR.'fileA2'));
assert(is_dir($inDir.DIRECTORY_SEPARATOR.'dirB') or mkdir($inDir.DIRECTORY_SEPARATOR.'dirB'));
assert(touch($inDir.DIRECTORY_SEPARATOR.'dirB'.DIRECTORY_SEPARATOR.'fileB1'));
assert(is_dir($inDir.DIRECTORY_SEPARATOR.'dirAC') or mkdir($inDir.DIRECTORY_SEPARATOR.'dirAC'));
assert(touch($inDir.DIRECTORY_SEPARATOR.'dirAC'.DIRECTORY_SEPARATOR.'fileAC1'));

// and a phar wrapper around it
$tmpphar = __DIR__.DIRECTORY_SEPARATOR.'bug54289.phar';
$pharA = new Phar($tmpphar);
$pharA->buildFromDirectory($inDir);

// we should be able to pull out a directory that's there
$outDir = __DIR__.DIRECTORY_SEPARATOR.'bug54289.extract.d';
$pharB = new Phar($tmpphar);
$pharB->extractTo($outDir, 'dirA', true);
assert(file_exists($outDir.DIRECTORY_SEPARATOR.'dirA'.DIRECTORY_SEPARATOR.'fileA1'));
assert(file_exists($outDir.DIRECTORY_SEPARATOR.'dirA'.DIRECTORY_SEPARATOR.'fileA2'));
assert(! is_dir($outDir.DIRECTORY_SEPARATOR.'dirAC'));

// but should not be able to pull out non-existent ones
try {
  $pharB->extractTo($outDir, 'dirX', true);
  echo 'failed to raise exception';
} catch (PharException $ex) {
}

// should be able to do by array, too
$pharB = new Phar($tmpphar);
$pharB->extractTo($outDir, [ 'dirA', 'dirB' ], true);

// but not an array with a bad member in it
try {
  $pharB = new Phar($tmpphar);
  $pharB->extractTo($outDir, [ 'dirA', 'dirX' ], true);
  echo 'failed to raise exception';
} catch (PharException $ex) {
}
echo 'done';
?>
--EXPECTF--
done
