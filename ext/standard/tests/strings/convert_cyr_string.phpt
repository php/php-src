--TEST--
basic convert_cyr_string() tests
--FILE--
<?php

var_dump(convert_cyr_string());
var_dump(convert_cyr_string(""));
var_dump(convert_cyr_string("", ""));
var_dump(convert_cyr_string("", "", ""));
var_dump(convert_cyr_string(array(), array(), array()));

var_dump(convert_cyr_string((binary)"[[[[[[", "q", "m"));
var_dump(convert_cyr_string((binary)"[[[[[[", "k", "w"));
var_dump(convert_cyr_string((binary)"[[[[[[", "m", "a"));
var_dump(convert_cyr_string((binary)"[[[[[[", "d", "i"));
var_dump(convert_cyr_string((binary)"[[[[[[", "w", "k"));
var_dump(convert_cyr_string((binary)"[[[[[[", "i", "q"));
var_dump(convert_cyr_string((binary)"", "d", "i"));

echo "Done\n";
?>
--EXPECTF--	
Warning: convert_cyr_string() expects exactly 3 parameters, 0 given in %s on line %d
NULL

Warning: convert_cyr_string() expects exactly 3 parameters, 1 given in %s on line %d
NULL

Warning: convert_cyr_string() expects exactly 3 parameters, 2 given in %s on line %d
NULL

Warning: convert_cyr_string(): Unknown source charset:  in %s on line %d

Warning: convert_cyr_string(): Unknown destination charset:  in %s on line %d
string(0) ""

Warning: convert_cyr_string() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: convert_cyr_string(): Unknown source charset: q in %s on line %d
string(6) "[[[[[["
string(6) "[[[[[["
string(6) "[[[[[["
string(6) "[[[[[["
string(6) "[[[[[["

Warning: convert_cyr_string(): Unknown destination charset: q in %s on line %d
string(6) "[[[[[["
string(0) ""
Done
