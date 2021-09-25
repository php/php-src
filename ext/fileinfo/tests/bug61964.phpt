--TEST--
Bug #61964 (finfo_open with directory cause invalid free)
--EXTENSIONS--
fileinfo
--FILE--
<?php

$magic_file = __DIR__ . DIRECTORY_SEPARATOR . 'magic';

$ret = @finfo_open(FILEINFO_NONE, $magic_file . ".non-exits");
var_dump($ret);

$dir = __DIR__ . "/bug61964";
@mkdir($dir);

$magic_file_copy = $dir . "/magic.copy";
$magic_file_copy2 = $magic_file_copy . "2";
copy($magic_file, $magic_file_copy);
copy($magic_file, $magic_file_copy2);

$ret = finfo_open(FILEINFO_NONE, $dir);
var_dump($ret);

$ret = @finfo_open(FILEINFO_NONE, $dir);
var_dump($ret);

$ret = @finfo_open(FILEINFO_NONE, $dir. "/non-exits-dir");
var_dump($ret);

// write some test files to test folder
file_put_contents($dir . "/test1.txt", "string\n> Core\n> Me");
file_put_contents($dir . "/test2.txt", "a\nb\n");
@mkdir($dir . "/test-inner-folder");

finfo_open(FILEINFO_NONE, $dir);
echo "DONE: testing dir with files\n";

rmdir($dir . "/test-inner-folder");
unlink($dir . "/test1.txt");
unlink($dir . "/test2.txt");

unlink($magic_file_copy);
unlink($magic_file_copy2);
rmdir($dir);
?>
--EXPECTF--
bool(false)%A
object(finfo)#%d (0) {
}
object(finfo)#%d (0) {
}
bool(false)%A
Warning: finfo_open(): offset `string' invalid in %sbug61964.php on line %d

Warning: finfo_open(): offset ` Core' invalid in %sbug61964.php on line %d

Warning: finfo_open(): offset ` Me' invalid in %sbug61964.php on line %d

Warning: finfo_open(): offset `a' invalid in %sbug61964.php on line %d

Warning: finfo_open(): offset `b' invalid in %sbug61964.php on line %d

Warning: finfo_open(): Failed to load magic database at "%sbug61964" in %sbug61964.php on line %d
DONE: testing dir with files
