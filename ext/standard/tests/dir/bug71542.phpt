--TEST--
Bug #71542 (disk_total_space does not work with relative paths)
--FILE--
<?php
chdir(__DIR__);
$dir = basename(getcwd());
chdir("..");
var_dump(
    disk_total_space($dir) !== false,
    disk_free_space($dir) !== false
);
?>
--EXPECT--
bool(true)
bool(true)
