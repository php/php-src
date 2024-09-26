--TEST--
Bug #68335: rmdir doesn't work with file:// stream wrapper
--FILE--
<?php
$dir = 'file://' . __DIR__ . '/test_68335_dir';
mkdir($dir);
var_dump(is_dir($dir));
rmdir($dir);
var_dump(is_dir($dir));
?>
--CLEAN--
<?php
$dir = __DIR__ . '/test_68335_dir';
@rmdir($dir);
?>
--EXPECT--
bool(true)
bool(false)
