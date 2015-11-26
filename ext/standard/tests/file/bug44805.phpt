--TEST--
Bug#44806 (rename() function is not portable to Windows)
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
    die('skip.. only for Windows');
}
?>
--FILE--
<?php
$dirname = dirname(__FILE__);
$file1 = $dirname . DIRECTORY_SEPARATOR . "file1.txt";
$file2 = $dirname . DIRECTORY_SEPARATOR . "file2.txt";

file_put_contents($file1, "this is file 1");
file_put_contents($file2, "this is file 2");

rename($file1, $file2);

echo "reading file 2: ";
readfile($file2);
if (file_exists($file1)) {
	unlink($file1);
}
if (file_exists($file2)) {
	unlink($file2);
}
?>
--EXPECT--
reading file 2: this is file 1
