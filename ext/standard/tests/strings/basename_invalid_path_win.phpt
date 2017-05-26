--TEST--
Test basename() function : usage variations with invalid paths
--SKIPIF--
<?php
if((substr(PHP_OS, 0, 3) != "WIN"))
  die('skip Run only on Windows"');
?>
--FILE--
<?php
/* Prototype: string basename ( string $path [, string $suffix] );
   Description: Given a string containing a path to a file,
                this function will return the base name of the file. 
                If the filename ends in suffix this will also be cut off.
*/

var_dump(basename("\377"));

echo "Done\n";

--EXPECTF--
string(1) "%c"
Done
