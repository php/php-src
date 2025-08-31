--TEST--
Bug #70019 Files extracted from archive may be placed outside of destination directory
--EXTENSIONS--
phar
zlib
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
--CLEAN--
<?php
$dir = __DIR__."/bug70019";
unlink("$dir/ThisIsATestFile.txt");
rmdir($dir);
?>
--EXPECT--
bool(true)
