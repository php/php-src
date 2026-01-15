--TEST--
Nullable object cast: basic functionality
--FILE--
<?php

var_dump((?object) null);

class Foo {
    public $bar = "baz";
}
var_dump((?object) new Foo());

var_dump((?object) ["key" => "value"]);

?>
--EXPECTF--
NULL
object(Foo)#%d (1) {
  ["bar"]=>
  string(3) "baz"
}
object(stdClass)#%d (1) {
  ["key"]=>
  string(5) "value"
}
