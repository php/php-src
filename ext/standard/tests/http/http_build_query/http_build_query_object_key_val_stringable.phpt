--TEST--
http_build_query() function with recursif object
--FILE--
<?php
class KeyValStringable {
    public $public = "input";
    protected $protected = "hello";
    private $private = "world";

    public function __toString(): string {
        return "Stringable";
    }
}

$o = new KeyValStringable();

var_dump(http_build_query($o));
?>
--EXPECTF--
Deprecated: http_build_query(): Passing an object for argument #1 $data to http_build_query() is deprecated, call get_object_vars() first instead in %s on line %d
string(12) "public=input"
