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
    try {
        var_dump(dirname("/foo/bar/baz", $i));
    } catch (\Error $e) {
        echo $e->getMessage() . "\n";
    }
}

var_dump(dirname("/foo/bar/baz", PHP_INT_MAX));
var_dump(dirname("g:/foo/bar/baz", PHP_INT_MAX));
var_dump(dirname("g:foo/bar/baz", PHP_INT_MAX));
?>
Done
--EXPECT--
Invalid argument, levels must be >= 1
string(8) "/foo/bar"
string(4) "/foo"
string(1) "\"
string(1) "\"
string(1) "\"
string(3) "g:\"
string(3) "g:."
Done
