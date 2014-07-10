--TEST--
Test open_basedir configuration for glob
--CONCURRENCY_GROUP--
open_basedir
--INI--
open_basedir=.
--FILE--
<?php
$dir = "globtest1";
$dir2 = "globtest2";
mkdir($dir);
mkdir($dir2);
chdir($dir);
var_dump(glob("../globtest*"));
?>
--CLEAN--
<?php
$dir = "globtest1";
$dir2 = "globtest2";
rmdir($dir);
rmdir($dir2);
?>
--EXPECT--
array(1) {
  [0]=>
  string(12) "../globtest1"
}

