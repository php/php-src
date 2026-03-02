--TEST--
Test dirname() function : usage variations
--SKIPIF--
<?php
if((substr(PHP_OS, 0, 3) != "WIN"))
  die('skip Windows only');
?>
--FILE--
<?php
for ($i=0 ; $i<5 ; $i++) {
    try {
        var_dump(dirname("/foo/bar/baz", $i));
    } catch (\ValueError $e) {
        echo $e->getMessage() . "\n";
    }
}

var_dump(dirname("/foo/bar/baz", PHP_INT_MAX));
var_dump(dirname("g:/foo/bar/baz", PHP_INT_MAX));
var_dump(dirname("g:foo/bar/baz", PHP_INT_MAX));
?>
--EXPECT--
dirname(): Argument #2 ($levels) must be greater than or equal to 1
string(8) "/foo/bar"
string(4) "/foo"
string(1) "\"
string(1) "\"
string(1) "\"
string(3) "g:\"
string(3) "g:."
