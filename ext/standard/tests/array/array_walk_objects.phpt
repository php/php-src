--TEST--
array_walk() and objects
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
array_walk($stdclass, "walk");

$t = new test;
array_walk($t, "walk");

$var = array();
array_walk($var, "walk");
$var = "";
try {
    array_walk($var, "walk");
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

echo "Done\n";
?>
--EXPECTF--
string(3) "foo"
string(3) "foo"
string(3) "bar"
string(3) "bar"
string(13) "%r\0%rtest%r\0%rvar_pri"
string(12) "test_private"
string(10) "%r\0%r*%r\0%rvar_pro"
string(14) "test_protected"
string(7) "var_pub"
string(11) "test_public"
array_walk(): Argument #1 ($array) must be of type array, string given
Done
