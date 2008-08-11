--TEST--
Bug #45181 (chdir() should clear relative entries in stat cache)
--FILE--
<?php
mkdir("bug45181_x");
var_dump(is_dir("bug45181_x"));
chdir("bug45181_x");
var_dump(is_dir("bug45181_x"));
?>
--CLEAN--
<?php
rmdir("bug45181_x");
?>
--EXPECT--
bool(true)
bool(false)
