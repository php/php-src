--TEST--
iconv_substr() with out of bounds offset
--SKIPIF--
<?php extension_loaded('iconv') or die('skip iconv extension is not available'); ?>
--FILE--
<?php

var_dump(iconv_substr("foo", 3));
var_dump(iconv_substr("foo", -3));
var_dump(iconv_substr("foo", 4));
var_dump(iconv_substr("foo", -4));
var_dump(iconv_substr("äöü", 3));
var_dump(iconv_substr("äöü", -3));
var_dump(iconv_substr("äöü", 4));
var_dump(iconv_substr("äöü", -4));
var_dump(iconv_substr("foo", 0, 3));
var_dump(iconv_substr("foo", 0, -3));
var_dump(iconv_substr("foo", 0, 4));
var_dump(iconv_substr("foo", 0, -4));
var_dump(iconv_substr("äöü", 0, 3));
var_dump(iconv_substr("äöü", 0, -3));
var_dump(iconv_substr("äöü", 0, 4));
var_dump(iconv_substr("äöü", 0, -4));
var_dump(iconv_substr("äöü", -4, 1));
var_dump(iconv_substr("äöü", -4, -1));
var_dump(iconv_substr("äöü", 2, -2));

?>
--EXPECT--
string(0) ""
string(3) "foo"
string(0) ""
string(3) "foo"
string(0) ""
string(6) "äöü"
string(0) ""
string(6) "äöü"
string(3) "foo"
string(0) ""
string(3) "foo"
string(0) ""
string(6) "äöü"
string(0) ""
string(6) "äöü"
string(0) ""
string(2) "ä"
string(4) "äö"
string(0) ""
