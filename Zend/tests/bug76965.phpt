--TEST--
Bug #76965 (INI_SCANNER_RAW doesn't strip trailing whitespace)
--FILE--
<?php
// the trailing whitespace is intentional
$ini = <<<END
1="foo"
2="bar" ; comment
3= baz
4= "foo;bar"
5= "foo" ; bar ; baz
6= "foo;bar" ; baz
7= foo"bar ; "ok
END;

var_dump(parse_ini_string($ini, false, INI_SCANNER_RAW));
?>
--EXPECT--
array(7) {
  [1]=>
  string(3) "foo"
  [2]=>
  string(3) "bar"
  [3]=>
  string(3) "baz"
  [4]=>
  string(7) "foo;bar"
  [5]=>
  string(3) "foo"
  [6]=>
  string(7) "foo;bar"
  [7]=>
  string(7) "foo"bar"
}
