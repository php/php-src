--TEST--
Non-null object cast: basic functionality
--FILE--
<?php

class Foo {
    public $bar = "baz";
}
var_dump((!object) new Foo());

var_dump((!object) ["key" => "value"]);

?>
--EXPECTF--
object(Foo)#%d (1) {
  ["bar"]=>
  string(3) "baz"
}
object(stdClass)#%d (1) {
  ["key"]=>
  string(5) "value"
}
