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
  [u"foo"]=>
  unicode(3) "abc"
  [u"bar"]=>
  unicode(3) "def"
  [u"fubar"]=>
  unicode(3) "ghi"
}
unicode(0) ""
unicode(0) ""
unicode(0) ""
unicode(3) "abc"
unicode(3) "def"
unicode(3) "ghi"
array(3) {
  [u"+foo"]=>
  unicode(3) "def"
  [u"-bar"]=>
  unicode(3) "jkl"
  [u"fubar"]=>
  unicode(0) ""
}
unicode(0) ""
unicode(0) ""
unicode(0) ""
unicode(0) ""
unicode(0) ""
unicode(0) ""
array(3) {
  [u"foo"]=>
  array(4) {
    [0]=>
    unicode(3) "abc"
    [1]=>
    unicode(3) "def"
    [2]=>
    unicode(3) "ghi"
    [3]=>
    unicode(0) ""
  }
  [u"bar"]=>
  array(1) {
    [0]=>
    unicode(0) ""
  }
  [u"fubar"]=>
  array(1) {
    [0]=>
    unicode(1) "="
  }
}
unicode(0) ""
unicode(0) ""
unicode(0) ""
array(4) {
  [0]=>
  unicode(3) "abc"
  [1]=>
  unicode(3) "def"
  [2]=>
  unicode(3) "ghi"
  [3]=>
  unicode(0) ""
}
array(1) {
  [0]=>
  unicode(0) ""
}
array(1) {
  [0]=>
  unicode(1) "="
}
