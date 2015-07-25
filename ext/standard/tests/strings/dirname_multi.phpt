--TEST--
Test dirname() function : usage variations
--FILE--
<?php
/* Prototype: string dirname ( string $path [, int nb]);
   Description: Returns directory name component of path.
*/
for ($i=1 ; $i<5 ; $i++) {
	var_dump(dirname("/foo/bar/baz", $i));
}
var_dump(dirname("/foo/bar/baz", PHP_INT_MAX));
?>
Done
--EXPECT--
string(8) "/foo/bar"
string(4) "/foo"
string(1) "/"
string(1) "/"
string(1) "/"
Done
