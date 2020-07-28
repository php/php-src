--TEST--
Test realpath() with relative paths
--FILE--
<?php
$file_path = __DIR__;
@mkdir("$file_path/realpath_basic4/home/test", 0777, true);
@symlink("$file_path/realpath_basic4/home", "$file_path/realpath_basic4/link1");
@symlink("$file_path/realpath_basic4/link1", "$file_path/realpath_basic4/link2");
echo "1. " . realpath("$file_path/realpath_basic4/link2") . "\n";
echo "2. " . realpath("$file_path/realpath_basic4/link2/test") . "\n";
?>
--CLEAN--
<?php
$file_path = __DIR__;
unlink("$file_path/realpath_basic4/link2");
unlink("$file_path/realpath_basic4/link1");
rmdir("$file_path/realpath_basic4/home/test");
rmdir("$file_path/realpath_basic4/home");
rmdir("$file_path/realpath_basic4");
?>
--EXPECTF--
1. %s%erealpath_basic4%ehome
2. %s%erealpath_basic4%ehome%etest
