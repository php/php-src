--TEST--
Test dirname() function : usage variations
--SKIPIF--
<?php
if((substr(PHP_OS, 0, 3) != "WIN"))
  die('skip Windows only');
?>
--FILE--
<?php
/* Prototype: string dirname ( string $path [, int nb]);
   Description: Returns directory name component of path.
*/
for ($i=0 ; $i<5 ; $i++) {
	var_dump(dirname("/foo/bar/baz", $i));
}
var_dump(dirname("/foo/bar/baz", PHP_INT_MAX));
var_dump(dirname("g:/foo/bar/baz", PHP_INT_MAX));
var_dump(dirname("g:foo/bar/baz", PHP_INT_MAX));
?>
Done
--EXPECTF--
Warning: dirname(): Invalid argument, levels must be >= 1 in %sdirname_multi_win.php on line %d
NULL
string(8) "/foo/bar"
string(4) "/foo"
string(1) "\"
string(1) "\"
string(1) "\"
string(3) "g:\"
string(3) "g:."
Done
