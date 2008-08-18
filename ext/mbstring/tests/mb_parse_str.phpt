--TEST--
mb_parse_str()
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--INI--
arg_separator.input=&
--FILE--
<?php
$queries = array(
	"foo=abc&bar=def",
	"%2bfoo=def&-bar=jkl",
	"foo[]=abc&foo[]=def&foo[]=ghi&bar[]=jkl"
);
function test($query) {
	$foo = '';
	$bar = '';
	mb_parse_str($query, $array);
	var_dump($array);
	var_dump($foo);
	var_dump($bar);
	mb_parse_str($query);
	var_dump($foo);
	var_dump($bar);
}
foreach ($queries as $query) {
	test($query);
}
?>
--EXPECT--
array(2) {
  [u"foo"]=>
  string(3) "abc"
  [u"bar"]=>
  string(3) "def"
}
unicode(0) ""
unicode(0) ""
unicode(3) "abc"
unicode(3) "def"
array(2) {
  [u"+foo"]=>
  string(3) "def"
  [u"-bar"]=>
  string(3) "jkl"
}
unicode(0) ""
unicode(0) ""
unicode(0) ""
unicode(0) ""
array(2) {
  [u"foo"]=>
  array(3) {
    [0]=>
    string(3) "abc"
    [1]=>
    string(3) "def"
    [2]=>
    string(3) "ghi"
  }
  [u"bar"]=>
  array(1) {
    [0]=>
    string(3) "jkl"
  }
}
unicode(0) ""
unicode(0) ""
array(3) {
  [0]=>
  string(3) "abc"
  [1]=>
  string(3) "def"
  [2]=>
  string(3) "ghi"
}
array(1) {
  [0]=>
  string(3) "jkl"
}
