--TEST--
array_walk_recursive() and objects
--FILE--
<?php

function walk($key, $value) {
    var_dump($value, $key);
}

class test {
    private $var_pri = "test_private";
    protected $var_pro = "test_protected";
    public $var_pub = "test_public";
}

$stdclass = new stdclass;
$stdclass->foo = "foo";
$stdclass->bar = "bar";
array_walk_recursive($stdclass, "walk");

$t = new test;
array_walk_recursive($t, "walk");

$var = array();
array_walk_recursive($var, "walk");
$var = "";
try {
    array_walk_recursive($var, "walk");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo "Done\n";
?>
--EXPECTF--
Deprecated: array_walk_recursive(): Passing an object for argument #1 $array to array_walk_recursive() is deprecated, call get_object_vars() first instead in %s on line %d
string(3) "foo"
string(3) "foo"
string(3) "bar"
string(3) "bar"

Deprecated: array_walk_recursive(): Passing an object for argument #1 $array to array_walk_recursive() is deprecated, call get_object_vars() first instead in %s on line %d
string(13) "%0test%0var_pri"
string(12) "test_private"
string(10) "%0*%0var_pro"
string(14) "test_protected"
string(7) "var_pub"
string(11) "test_public"
TypeError: array_walk_recursive(): Argument #1 ($array) must be of type array, string given
Done
