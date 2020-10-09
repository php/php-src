--TEST--
SPL: DirectoryIterator::getExtension() basic test
--FILE--
<?php
$dir = __DIR__ . DIRECTORY_SEPARATOR . md5('DirectoryIterator::getExtension') . DIRECTORY_SEPARATOR;

if (!mkdir($dir)) {
    die('Failed to create test directory');
}

$files = array('test.txt', 'test.extension', 'test');
foreach ($files as $file) {
    touch($dir . $file);
}

$dit_exts = array();
$nfo_exts = array();

foreach (new DirectoryIterator($dir) as $file) {
    if ($file->isDot()) {
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
$files = array('test.txt', 'test.extension', 'test');
foreach ($files as $file) {
    unlink($dir . $file);
}
rmdir($dir);
?>
--EXPECT--
bool(true)
array(3) {
  [0]=>
  string(0) ""
  [1]=>
  string(9) "extension"
  [2]=>
  string(3) "txt"
}
