--TEST--
http_build_query() function with object
--FILE--
<?php
class KeyVal {
    public $public = "input";
    protected $protected = "hello";
    private $private = "world";
}

$o = new KeyVal();

// Percent encoded "public=input"
var_dump(http_build_query($o));
?>
--EXPECT--
string(12) "public=input"
