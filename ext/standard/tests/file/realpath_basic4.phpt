--TEST--
Test realpath() with relative paths
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip no symlinks on Windows');
}
?>
--FILE--
<?php
$file_path = dirname(__FILE__);
@mkdir("$file_path/realpath_basic/home/test", 0777, true);
@symlink("$file_path/realpath_basic/home", "$file_path/realpath_basic/link1");
@symlink("$file_path/realpath_basic/link1", "$file_path/realpath_basic/link2");
echo "1. " . realpath("$file_path/realpath_basic/link2") . "\n";
echo "2. " . realpath("$file_path/realpath_basic/link2/test") . "\n";
?>
--CLEAN--
<?php
$file_path = dirname(__FILE__);
unlink("$file_path/realpath_basic/link2");
unlink("$file_path/realpath_basic/link1");
rmdir("$file_path/realpath_basic/home/test");
rmdir("$file_path/realpath_basic/home");
rmdir("$file_path/realpath_basic");
?>
--EXPECTF--
1. %s%erealpath_basic%ehome
2. %s%erealpath_basic%ehome%etest
