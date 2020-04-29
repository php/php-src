--TEST--
Test basename() function : usage variations with invalid paths
--SKIPIF--
<?php
if((substr(PHP_OS, 0, 3) == "WIN"))
  die('skip not for Windows"');
?>
--FILE--
<?php
/* Prototype: string basename ( string $path [, string $suffix] );
   Description: Given a string containing a path to a file,
                this function will return the base name of the file.
                If the filename ends in suffix this will also be cut off.
*/

setlocale(LC_CTYPE, "C");
var_dump(bin2hex(basename("\xff")));
var_dump(bin2hex(basename("a\xffb")));

echo "Done\n";
--EXPECT--
string(2) "ff"
string(6) "61ff62"
Done
