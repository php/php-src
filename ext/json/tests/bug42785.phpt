--TEST--
Bug #42785 (Incorrect formatting of double values with non-english locales)
--INI--
serialize_precision=-1
--SKIPIF--
<?php
if (!extension_loaded('json')) die('skip');
if (!setlocale(LC_CTYPE, "de_DE", "de", "german", "ge", "de_DE.ISO8859-1", "ISO8859-1")) {
	die("skip locale needed for this test is not supported on this platform");
}
?>
--FILE--
<?php
setlocale(LC_ALL, "de_DE", "de", "german", "ge", "de_DE.ISO8859-1", "ISO8859-1");

$foo = array(100.10,"bar");
var_dump(json_encode($foo));

class bar {}
$bar1 = new bar;
$bar1->a = 100.10;
$bar1->b = "foo";
var_dump(json_encode($bar1));
?>
--EXPECT--
string(13) "[100.1,"bar"]"
string(21) "{"a":100.1,"b":"foo"}"
