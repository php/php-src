--TEST--
filter_data() and FC_CALLBACK
--GET--

--FILE--
<?php

/* Simple callback function */
function test($var) {
	return strtoupper($var);
}
	
var_dump(filter_data("data", FC_CALLBACK, "test"));
var_dump(filter_data("~!@#$%^&*()_QWERTYUIOPASDFGHJKLZXCVBNM<>>?\"}{:", FC_CALLBACK, "test"));
var_dump(filter_data("", FC_CALLBACK, "test"));
var_dump(filter_data("qwe", FC_CALLBACK, "no such func"));
var_dump(filter_data("qwe", FC_CALLBACK, ""));
var_dump(filter_data("qwe", FC_CALLBACK));

/* Simple class method callback */
class test_class {
	static function test ($var) {
		return strtolower($var);
	}
}

var_dump(filter_data("dAtA", FC_CALLBACK, array("test_class", "test")));
var_dump(filter_data("~!@#$%^&*()_QWERTYUIOPASDFGHJKLZXCVBNM<>>?\"}{:", FC_CALLBACK, array("test_class","test")));
var_dump(filter_data("", FC_CALLBACK, array("test_class","test")));

/* empty function without return value */
function test1($var) {
}
	
var_dump(filter_data("data", FC_CALLBACK, "test1"));
var_dump(filter_data("~!@#$%^&*()_QWERTYUIOPASDFGHJKLZXCVBNM<>>?\"}{:", FC_CALLBACK, "test1"));
var_dump(filter_data("", FC_CALLBACK, "test1"));

/* attempting to change data by reference */
function test2(&$var) {
	$var = 1;
}
	
var_dump(filter_data("data", FC_CALLBACK, "test2"));
var_dump(filter_data("~!@#$%^&*()_QWERTYUIOPASDFGHJKLZXCVBNM<>>?\"}{:", FC_CALLBACK, "test2"));
var_dump(filter_data("", FC_CALLBACK, "test2"));

/* unsetting data */
function test3(&$var) {
	unset($var);
}
	
var_dump(filter_data("data", FC_CALLBACK, "test3"));
var_dump(filter_data("~!@#$%^&*()_QWERTYUIOPASDFGHJKLZXCVBNM<>>?\"}{:", FC_CALLBACK, "test3"));
var_dump(filter_data("", FC_CALLBACK, "test3"));

/* unset data and return value */
function test4(&$var) {
	unset($var);
	return 1;
}
	
var_dump(filter_data("data", FC_CALLBACK, "test4"));

/* thrown exception in the callback */
function test5(&$var) {
	throw new Exception("test");
}

try {
	var_dump(filter_data("data", FC_CALLBACK, "test5"));
} catch (Exception $e) {
	var_dump($e->getMessage());
}

echo "Done\n";
?>
--EXPECTF--	
string(4) "DATA"
string(46) "~!@#$%^&*()_QWERTYUIOPASDFGHJKLZXCVBNM<>>?"}{:"
string(0) ""
PHP Warning:  filter_data(): First argument is expected to be a valid callback in %s on line %d

Warning: filter_data(): First argument is expected to be a valid callback in %s on line %d
NULL
PHP Warning:  filter_data(): First argument is expected to be a valid callback in %s on line %d

Warning: filter_data(): First argument is expected to be a valid callback in %s on line %d
NULL
PHP Warning:  filter_data(): First argument is expected to be a valid callback in %s on line %d

Warning: filter_data(): First argument is expected to be a valid callback in %s on line %d
NULL
string(4) "data"
string(46) "~!@#$%^&*()_qwertyuiopasdfghjklzxcvbnm<>>?"}{:"
string(0) ""
NULL
NULL
NULL
NULL
NULL
NULL
NULL
NULL
NULL
int(1)
string(4) "test"
Done
