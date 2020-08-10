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

try {
    filter_var("qwe", FILTER_CALLBACK, array("options"=>"no such func"));
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    filter_var("qwe", FILTER_CALLBACK, array("options"=>""));
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    filter_var("qwe", FILTER_CALLBACK);
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

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

/* thrown exception in the callback */
function test3($var) {
    throw new Exception("test");
}

try {
    var_dump(filter_var("data", FILTER_CALLBACK, array("options"=>"test3")));
} catch (Exception $e) {
    var_dump($e->getMessage());
}

echo "Done\n";
?>
--EXPECTF--
string(4) "DATA"
string(46) "~!@#$%^&*()_QWERTYUIOPASDFGHJKLZXCVBNM<>>?"}{:"
string(0) ""
filter_var(): Option must be a valid callback
filter_var(): Option must be a valid callback
filter_var(): Option must be a valid callback
string(4) "data"
string(46) "~!@#$%^&*()_qwertyuiopasdfghjklzxcvbnm<>>?"}{:"
string(0) ""
NULL
NULL
NULL

Warning: test2(): Argument #1 ($var) must be passed by reference, value given in %s on line %d
NULL

Warning: test2(): Argument #1 ($var) must be passed by reference, value given in %s on line %d
NULL

Warning: test2(): Argument #1 ($var) must be passed by reference, value given in %s on line %d
NULL
string(4) "test"
Done
