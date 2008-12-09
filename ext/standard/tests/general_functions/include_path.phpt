--TEST--
*_include_path() tests
--INI--
include_path=.
--FILE--
<?php

var_dump(get_include_path());
var_dump(get_include_path("var"));

var_dump(restore_include_path());
var_dump(restore_include_path(""));


var_dump(set_include_path());
var_dump(get_include_path());
var_dump(set_include_path("var"));
var_dump(get_include_path());

var_dump(restore_include_path());
var_dump(get_include_path());

var_dump(set_include_path(".:/path/to/dir"));
var_dump(get_include_path());

var_dump(restore_include_path());
var_dump(get_include_path());

var_dump(set_include_path(""));
var_dump(get_include_path());

var_dump(restore_include_path());
var_dump(get_include_path());

var_dump(set_include_path(array()));
var_dump(get_include_path());

var_dump(restore_include_path());
var_dump(get_include_path());


echo "Done\n";
?>
--EXPECTF--	
string(1) "."

Warning: get_include_path() expects exactly 0 parameters, 1 given in %s on line %d
NULL
NULL

Warning: restore_include_path() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: set_include_path() expects exactly 1 parameter, 0 given in %s on line %d
NULL
string(1) "."
string(1) "."
string(3) "var"
NULL
string(1) "."
string(1) "."
string(14) ".:/path/to/dir"
NULL
string(1) "."
bool(false)
string(1) "."
NULL
string(1) "."

Warning: set_include_path() expects parameter 1 to be string, array given in %s on line %d
NULL
string(1) "."
NULL
string(1) "."
Done
