--TEST--
Bug #78296 (is_file fails to detect file)
--FILE--
<?php
$dir = str_pad(__DIR__ . '/bug78296', 250, '_');
var_dump(mkdir($dir));
var_dump(is_dir($dir));
?>
--EXPECT--
bool(true)
bool(true)
--CLEAN--
<?php
$dir = str_pad(__DIR__ . '/bug78296', 250, '_');
rmdir($dir);
?>
