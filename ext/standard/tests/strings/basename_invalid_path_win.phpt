--TEST--
Test basename() function : usage variations with invalid paths
--SKIPIF--
<?php
if((substr(PHP_OS, 0, 3) != "WIN"))
  die('skip Run only on Windows"');
?>
--FILE--
<?php
var_dump(basename("\377"));

echo "Done\n";
?>
--EXPECTF--
string(1) "%c"
Done
