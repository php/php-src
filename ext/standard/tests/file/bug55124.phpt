--TEST--
Bug #55124 (recursive mkdir fails with current (dot) directory in path)
--FILE--
<?php
$old_dir_path = getcwd();
chdir(__DIR__);
mkdir('a/./b', 0755, true);
if (is_dir('a/b')) {
    rmdir('a/b');
}
if (is_dir('./a')) {
    rmdir('a');
}
chdir($old_dir_path);
echo "OK";
?>
--EXPECT--
OK
