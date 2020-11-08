--TEST--
Test basename() function : usage variations with invalid paths
--SKIPIF--
<?php
if((substr(PHP_OS, 0, 3) == "WIN"))
  die('skip not for Windows"');
?>
--FILE--
<?php
setlocale(LC_CTYPE, "C");
var_dump(bin2hex(basename("\xff")));
var_dump(bin2hex(basename("a\xffb")));

echo "Done\n";
?>
--EXPECT--
string(2) "ff"
string(6) "61ff62"
Done
