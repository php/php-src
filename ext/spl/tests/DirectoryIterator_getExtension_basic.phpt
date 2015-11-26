--TEST--
SPL: DirectoryIterator::getExtension() basic test
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip.. only for Unix');
}
?>
--FILE--
<?php
$dir = __DIR__ . DIRECTORY_SEPARATOR . md5('DirectoryIterator::getExtension') . DIRECTORY_SEPARATOR;
mkdir($dir);

$files = array('test.txt', 'test.extension', 'test..', 'test.', 'test');
foreach ($files as $file) {
    touch($dir . $file);
}

$dit_exts = array();
$nfo_exts = array();
$skip = array('.', '..');

foreach (new DirectoryIterator($dir) as $file) {
    if (in_array($file->getFilename(), $skip)) {
        continue;
    }
    $dit_exts[] = $file->getExtension();
    $nfo_exts[] = pathinfo($file->getFilename(), PATHINFO_EXTENSION);
}
var_dump($dit_exts === $nfo_exts);
sort($dit_exts);
var_dump($dit_exts);
?>
--CLEAN--
<?php
$dir   = __DIR__ . DIRECTORY_SEPARATOR . md5('DirectoryIterator::getExtension') . DIRECTORY_SEPARATOR;
$files = array('test.txt', 'test.extension', 'test..', 'test.', 'test');
foreach ($files as $file) {
    unlink($dir . $file);
}
rmdir($dir);
?>
--EXPECTF--
bool(true)
array(5) {
  [0]=>
  string(0) ""
  [1]=>
  string(0) ""
  [2]=>
  string(0) ""
  [3]=>
  string(9) "extension"
  [4]=>
  string(3) "txt"
}

