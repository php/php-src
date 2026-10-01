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
--EXPECTF--
Deprecated: http_build_query(): Passing an object for argument #1 $data to http_build_query() is deprecated, call get_object_vars() first instead in %s on line %d

Deprecated: http_build_query(): object values within argument #1 $data to http_build_query() being interpreted as arrays is deprecated, instead the $data argument should be preprocessed with get_object_vars() in %s on line %d
string(0) ""
