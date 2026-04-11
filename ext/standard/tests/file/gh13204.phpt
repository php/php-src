--TEST--
GH-13204 (glob() fails if square bracket is in current directory)
--FILE--
<?php
chdir(__DIR__ . '/gh13204[brackets]/');
var_dump(glob('./*'));
?>
--EXPECT--
array(1) {
  [0]=>
  string(11) "./empty.txt"
}
