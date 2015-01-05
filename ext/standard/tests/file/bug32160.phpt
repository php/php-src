--TEST--
Bug #32160 (copying a file into itself leads to data loss)
--FILE--
<?php
$path = dirname(__FILE__) . "/bug32160.txt";
var_dump(copy($path, $path));
chdir(dirname(__FILE__));
var_dump(copy($path, "bug32160.txt"));
var_dump(copy("bug32160.txt", "bug32160.txt"));
?>
--EXPECT--
bool(false)
bool(false)
bool(false)
