--TEST--
GH-21231: Unsound SCCP for partial objects with hooks
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
--FILE--
<?php

#[AllowDynamicProperties]
class Foo {
    public $prop { get => 43; set => $value; }
}

function test() {
    $obj = new Foo;
    $obj->prop = 42;
    return $obj->prop;
}

var_dump(test());

?>
--EXPECT--
int(43)
