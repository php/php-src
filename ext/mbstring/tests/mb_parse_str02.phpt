--TEST--
mb_parse_str() test 2
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--INI--
arg_separator.input=&#
--FILE--
<?php
$queries = array(
	"foo=abc#bar=def&fubar=ghi",
	"%2bfoo=def&-bar=jkl#+fubar",
	"  foo[]=abc&foo[]=def#foo[]=ghi#bar[]=#foo[]&fubar[]=="
);
function test($query) {
	$foo = '';
	$bar = '';
	$fubar = '';
	mb_parse_str($query, $array);
	var_dump($array);
	var_dump($foo);
	var_dump($bar);
	var_dump($fubar);
	mb_parse_str($query);
	var_dump($foo);
	var_dump($bar);
	var_dump($fubar);
}
foreach ($queries as $query) {
	test($query);
}
?>
--EXPECT--
array(3) {
  ["foo"]=>
  string(3) "abc"
  ["bar"]=>
  string(3) "def"
  ["fubar"]=>
  string(3) "ghi"
}
string(0) ""
string(0) ""
string(0) ""
string(3) "abc"
string(3) "def"
string(3) "ghi"
array(3) {
  ["+foo"]=>
  string(3) "def"
  ["-bar"]=>
  string(3) "jkl"
  ["fubar"]=>
  string(0) ""
}
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
array(3) {
  ["foo"]=>
  array(4) {
    [0]=>
    string(3) "abc"
    [1]=>
    string(3) "def"
    [2]=>
    string(3) "ghi"
    [3]=>
    string(0) ""
  }
  ["bar"]=>
  array(1) {
    [0]=>
    string(0) ""
  }
  ["fubar"]=>
  array(1) {
    [0]=>
    string(1) "="
  }
}
string(0) ""
string(0) ""
string(0) ""
array(4) {
  [0]=>
  string(3) "abc"
  [1]=>
  string(3) "def"
  [2]=>
  string(3) "ghi"
  [3]=>
  string(0) ""
}
array(1) {
  [0]=>
  string(0) ""
}
array(1) {
  [0]=>
  string(1) "="
}
