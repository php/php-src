--TEST--
Bug #73971 Filename got limited to MAX_PATH on Win32 when scan directory
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
  die("skip Valid only on Windows");
}
?>
--FILE--
<?php
$base = __DIR__ . DIRECTORY_SEPARATOR . "bug73971";
$filename =  $base . DIRECTORY_SEPARATOR . str_repeat('テスト', 48); // 144 glyph here, less than 256

mkdir($base);
mkdir($filename); // created correctly

var_dump(basename($filename)); // 432 bytes here, more than 256

echo "\ntest dir()\n";
$d = dir($base);
while (false !== ($entry = $d->read())) {
    var_dump($entry);
}
$d->close();

echo "\ntest DirectoryIterator\n";
$dir = new DirectoryIterator($base);
foreach ($dir as $finfo) {
	var_dump($finfo->getFilename());
}

?>
==DONE==
--CLEAN--
<?php
$base = __DIR__ . DIRECTORY_SEPARATOR . "bug73971";
$filename =  $base . DIRECTORY_SEPARATOR . str_repeat('テスト', 48);

rmdir($filename);
rmdir($base);
?>
--EXPECT--
string(432) "テストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテスト"

test dir()
string(1) "."
string(2) ".."
string(432) "テストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテスト"

test DirectoryIterator
string(1) "."
string(2) ".."
string(432) "テストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテストテスト"
==DONE==
