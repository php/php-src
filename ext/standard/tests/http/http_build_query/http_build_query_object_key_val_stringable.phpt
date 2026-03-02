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
--EXPECT--
string(12) "public=input"
