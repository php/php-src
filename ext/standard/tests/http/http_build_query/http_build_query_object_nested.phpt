--TEST--
http_build_query() function with nested object
--FILE--
<?php
class KeyVal {
    public $public = "input";
    protected $protected = "hello";
    private $private = "world";
}

$o = new KeyVal();
$nested = new KeyVal();

$o->public = $nested;

// Percent encoded "public[public]=input"
var_dump(http_build_query($o));
?>
--EXPECT--
string(24) "public%5Bpublic%5D=input"
