--TEST--
#[\NoSerialize]: Traits properties test.
--FILE--
<?php

trait T {
    #[\NoSerialize]
    public $a = 'a';
    public $b = 'b';
}

class Base {
    use T;
}

$base = new Base;
echo serialize($base), PHP_EOL;
var_dump(unserialize('O:4:"Base":2:{s:1:"a";s:1:"a";s:1:"b";s:1:"b";}'));

?>
--EXPECTF--
O:4:"Base":1:{s:1:"b";s:1:"b";}
object(Base)#2 (2) {
  ["a"]=>
  string(1) "a"
  ["b"]=>
  string(1) "b"
}
