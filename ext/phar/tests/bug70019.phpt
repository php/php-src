--TEST--
Bug #70019 Files extracted from archive may be placed outside of destination directory
--SKIPIF--
<?php if (!extension_loaded("phar") || !extension_loaded('zlib')) die("skip"); ?>
--FILE--
<?php
$dir = __DIR__."/bug70019";
$phar = new PharData(__DIR__."/bug70019.zip");
if(!is_dir($dir)) {
  mkdir($dir);
}
$phar->extractTo($dir);
var_dump(file_exists("$dir/ThisIsATestFile.txt"));
?>
===DONE===
--CLEAN--
<?php
$dir = __DIR__."/bug70019";
unlink("$dir/ThisIsATestFile.txt");
rmdir($dir);
?>
--EXPECTF--
bool(true)
===DONE===
