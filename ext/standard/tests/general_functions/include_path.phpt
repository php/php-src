--TEST--
*_include_path() tests
--INI--
include_path=.
--FILE--
<?php

var_dump(get_include_path());

var_dump(restore_include_path());

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

var_dump(restore_include_path());
var_dump(get_include_path());


echo "Done\n";
?>
--EXPECTF--
string(1) "."

Deprecated: Function restore_include_path() is deprecated in %s on line %d
NULL
string(1) "."
string(3) "var"

Deprecated: Function restore_include_path() is deprecated in %s on line %d
NULL
string(1) "."
string(1) "."
string(14) ".:/path/to/dir"

Deprecated: Function restore_include_path() is deprecated in %s on line %d
NULL
string(1) "."
bool(false)
string(1) "."

Deprecated: Function restore_include_path() is deprecated in %s on line %d
NULL
string(1) "."

Deprecated: Function restore_include_path() is deprecated in %s on line %d
NULL
string(1) "."
Done
