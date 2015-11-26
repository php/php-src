--TEST--
filter_var() and FILTER_CALLBACK
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--FILE--
<?php

/* Simple callback function */
function test($var) {
	return strtoupper($var);
}
	
var_dump(filter_var("data", FILTER_CALLBACK, array("options"=>"test")));
var_dump(filter_var("~!@#$%^&*()_QWERTYUIOPASDFGHJKLZXCVBNM<>>?\"}{:", FILTER_CALLBACK, array("options"=>"test")));
var_dump(filter_var("", FILTER_CALLBACK, array("options"=>"test")));
var_dump(filter_var("qwe", FILTER_CALLBACK, array("options"=>"no such func")));
var_dump(filter_var("qwe", FILTER_CALLBACK, array("options"=>"")));
var_dump(filter_var("qwe", FILTER_CALLBACK));

/* Simple class method callback */
class test_class {
	static function test ($var) {
		return strtolower($var);
	}
}

var_dump(filter_var("dAtA", FILTER_CALLBACK, array("options"=>array("test_class", "test"))));
var_dump(filter_var("~!@#$%^&*()_QWERTYUIOPASDFGHJKLZXCVBNM<>>?\"}{:", FILTER_CALLBACK, array("options"=>array("test_class","test"))));
var_dump(filter_var("", FILTER_CALLBACK, array("options"=>array("test_class","test"))));

/* empty function without return value */
function test1($var) {
}
	
var_dump(filter_var("data", FILTER_CALLBACK, array("options"=>"test1")));
var_dump(filter_var("~!@#$%^&*()_QWERTYUIOPASDFGHJKLZXCVBNM<>>?\"}{:", FILTER_CALLBACK, array("options"=>"test1")));
var_dump(filter_var("", FILTER_CALLBACK, array("options"=>"test1")));

/* attempting to change data by reference */
function test2(&$var) {
	$var = 1;
}
	
var_dump(filter_var("data", FILTER_CALLBACK, array("options"=>"test2")));
var_dump(filter_var("~!@#$%^&*()_QWERTYUIOPASDFGHJKLZXCVBNM<>>?\"}{:", FILTER_CALLBACK, array("options"=>"test2")));
var_dump(filter_var("", FILTER_CALLBACK, array("options"=>"test2")));

/* unsetting data */
function test3(&$var) {
	unset($var);
}
	
var_dump(filter_var("data", FILTER_CALLBACK, array("options"=>"test3")));
var_dump(filter_var("~!@#$%^&*()_QWERTYUIOPASDFGHJKLZXCVBNM<>>?\"}{:", FILTER_CALLBACK, array("options"=>"test3")));
var_dump(filter_var("", FILTER_CALLBACK, array("options"=>"test3")));

/* unset data and return value */
function test4(&$var) {
	unset($var);
	return 1;
}
	
var_dump(filter_var("data", FILTER_CALLBACK, array("options"=>"test4")));

/* thrown exception in the callback */
function test5(&$var) {
	throw new Exception("test");
}

try {
	var_dump(filter_var("data", FILTER_CALLBACK, array("options"=>"test5")));
} catch (Exception $e) {
	var_dump($e->getMessage());
}

echo "Done\n";
?>
--EXPECTF--	
string(4) "DATA"
string(46) "~!@#$%^&*()_QWERTYUIOPASDFGHJKLZXCVBNM<>>?"}{:"
string(0) ""

Warning: filter_var(): First argument is expected to be a valid callback in %s on line %d
NULL

Warning: filter_var(): First argument is expected to be a valid callback in %s on line %d
NULL

Warning: filter_var(): First argument is expected to be a valid callback in %s on line %d
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
