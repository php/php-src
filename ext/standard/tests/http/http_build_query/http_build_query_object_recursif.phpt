--TEST--
http_build_query() function with recursif object
--FILE--
<?php
class KeyVal {
    public $public = "input";
    protected $protected = "hello";
    private $private = "world";
}

$o = new KeyVal();
$o->public = $o;

var_dump(http_build_query($o));
?>
--EXPECT--
string(0) ""
