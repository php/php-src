--TEST--
builtin functions tests
--FILE--
<?php

var_dump(get_resource_type());
var_dump(get_resource_type(""));
$fp = fopen(__FILE__, "r");
var_dump(get_resource_type($fp));
fclose($fp);
var_dump(get_resource_type($fp));

var_dump(gettype(get_loaded_extensions()));
var_dump(count(get_loaded_extensions()));
var_dump(gettype(get_loaded_extensions(true)));
var_dump(count(get_loaded_extensions(true)));
var_dump(get_loaded_extensions(true, true));

define("USER_CONSTANT", "test");

var_dump(gettype(get_defined_constants(true)));
var_dump(gettype(get_defined_constants()));
var_dump(count(get_defined_constants()));
var_dump(count(get_defined_constants(true, true)));

function test () {
}

var_dump(gettype(get_defined_functions()));
var_dump(count(get_defined_functions()));
var_dump(count(get_defined_functions(true)));

var_dump(gettype(get_declared_interfaces()));
var_dump(count(get_declared_interfaces()));
var_dump(count(get_declared_interfaces(true)));

var_dump(get_extension_funcs());
var_dump(get_extension_funcs(true));
var_dump(gettype(get_extension_funcs("standard")));
var_dump(count(get_extension_funcs("standard")));
var_dump(gettype(get_extension_funcs("zend")));
var_dump(count(get_extension_funcs("zend")));


echo "Done\n";
?>
--EXPECTF--	
Warning: get_resource_type() expects exactly 1 parameter, 0 given in %s on line 3
NULL

Warning: get_resource_type() expects parameter 1 to be resource, string given in %s on line 4
NULL
string(%d) "stream"
string(%d) "Unknown"
string(%d) "array"
int(%d)
string(%d) "array"
int(%d)
array(0) {
}
string(%d) "array"
string(%d) "array"
int(%d)
int(%d)
string(%d) "array"
int(%d)
int(%d)
string(%d) "array"
int(%d)
int(%d)

Warning: get_extension_funcs() expects exactly 1 parameter, 0 given in %s on line 34
NULL
bool(false)
string(%d) "array"
int(%d)
string(%d) "array"
int(%d)
Done
