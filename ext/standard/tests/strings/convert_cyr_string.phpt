--TEST--
basic convert_cyr_string() tests
--FILE--
<?php

var_dump(convert_cyr_string("", "", ""));
var_dump(convert_cyr_string(array(), array(), array()));

var_dump(convert_cyr_string("[[[[[[", "q", "m"));
var_dump(convert_cyr_string("[[[[[[", "k", "w"));
var_dump(convert_cyr_string("[[[[[[", "m", "a"));
var_dump(convert_cyr_string("[[[[[[", "d", "i"));
var_dump(convert_cyr_string("[[[[[[", "w", "k"));
var_dump(convert_cyr_string("[[[[[[", "i", "q"));
var_dump(convert_cyr_string("", "d", "i"));

echo "Done\n";
?>
--EXPECTF--
Deprecated: Function convert_cyr_string() is deprecated in %s on line %d

Warning: convert_cyr_string(): Unknown source charset:  in %s on line %d

Warning: convert_cyr_string(): Unknown destination charset:  in %s on line %d
string(0) ""

Deprecated: Function convert_cyr_string() is deprecated in %s on line %d

Warning: convert_cyr_string() expects parameter 1 to be string, array given in %s on line %d
NULL

Deprecated: Function convert_cyr_string() is deprecated in %s on line %d

Warning: convert_cyr_string(): Unknown source charset: q in %s on line %d
string(6) "[[[[[["

Deprecated: Function convert_cyr_string() is deprecated in %s on line %d
string(6) "[[[[[["

Deprecated: Function convert_cyr_string() is deprecated in %s on line %d
string(6) "[[[[[["

Deprecated: Function convert_cyr_string() is deprecated in %s on line %d
string(6) "[[[[[["

Deprecated: Function convert_cyr_string() is deprecated in %s on line %d
string(6) "[[[[[["

Deprecated: Function convert_cyr_string() is deprecated in %s on line %d

Warning: convert_cyr_string(): Unknown destination charset: q in %s on line %d
string(6) "[[[[[["

Deprecated: Function convert_cyr_string() is deprecated in %s on line %d
string(0) ""
Done
