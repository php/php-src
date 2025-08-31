--TEST--
FileSystemIterator without SKIP_DOTS
--FILE--
<?php

$dir = __DIR__ . '/filesystemiterator_no_skip_dots';
mkdir($dir);
touch($dir . '/file');

$it = new FileSystemIterator($dir, 0);
$files = [];
foreach ($it as $f) {
    $files[] = $f->getFileName();
}
sort($files);
var_dump($files);

?>
--CLEAN--
<?php
$dir = __DIR__ . '/filesystemiterator_no_skip_dots';
unlink($dir . '/file');
rmdir($dir);
?>
--EXPECT--
array(3) {
  [0]=>
  string(1) "."
  [1]=>
  string(2) ".."
  [2]=>
  string(4) "file"
}
