--TEST--
Bug #54289 Phar::extractTo() does not accept specific directories to be extracted
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly = 0
--FILE--
<?php
// put our test fixtures into a far
$base  = __DIR__.DIRECTORY_SEPARATOR.'bug54289'.DIRECTORY_SEPARATOR;
$inDir = $base.'in';
$phar  = $base.'test.phar';
$pharA = new Phar($phar);
$pharA->buildFromDirectory($inDir);

// we should be able to pull out a directory that's there, but none that share
// the same prefix
$outDir = $base.'out';
$pharB = new Phar($phar);
$pharB->extractTo($outDir, 'dirA/', true);
var_dump(file_exists($outDir.DIRECTORY_SEPARATOR.'dirA'.DIRECTORY_SEPARATOR.'fileA'));
var_dump(file_exists($outDir.DIRECTORY_SEPARATOR.'dirA'.DIRECTORY_SEPARATOR.'fileB'));
var_dump(is_dir($outDir.DIRECTORY_SEPARATOR.'dirAB'));

// should also not be able to pull out non-existent ones
try {
  $pharB->extractTo($outDir, 'dirX/', true);
  echo 'failed to throw expected exception';
} catch (PharException $ex) {
}

// should also not be able to pull out /, because paths are not "rooted" that way
try {
  $pharB->extractTo($outDir, '/', true);
  echo 'failed to throw expected exception';
} catch (PharException $ex) {
}

// should be able to do by array, too
$pharB = new Phar($phar);
$pharB->extractTo($outDir, [ 'dirA/', 'dirAB/' ], true);

// but not an array with a bad member in it
try {
  $pharB = new Phar($phar);
  $pharB->extractTo($outDir, [ 'dirA/', 'dirX/' ], true);
  echo 'failed to throw expected exception';
} catch (PharException $ex) {
}

echo 'done';
?>
--CLEAN--
<?php
$base   = __DIR__.DIRECTORY_SEPARATOR.'bug54289'.DIRECTORY_SEPARATOR;
$phar   = $base.'test.phar';
$outDir = $base.'out';
unlink($phar);
$iter = new \RecursiveIteratorIterator(new \RecursiveDirectoryIterator(
  $outDir, \FilesystemIterator::SKIP_DOTS|\FilesystemIterator::UNIX_PATHS),
  \RecursiveIteratorIterator::CHILD_FIRST);
foreach ($iter as $value) {
    $value->isFile() ? unlink($value) : rmdir($value);
}
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
done
