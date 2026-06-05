--TEST--
FilesystemIterator without SKIP_DOTS
--FILE--
<?php

$dir = __DIR__ . '/filesystemiterator_no_skip_dots';
mkdir($dir);
touch($dir . '/file');

$it = new FilesystemIterator($dir, 0);
$files = [];
foreach ($it as $f) {
    $files[] = $f->getFilename();
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
