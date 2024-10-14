--TEST--
Bug #30791 (magic methods (__sleep/__wakeup/__toString) call __call if object is overloaded)
--FILE--
<?php

class a
{
    public $a = 4;
    function __call($name, $args) {
        echo __METHOD__, "\n";
    }
}

$b = new a;
var_dump($b);
$c = unserialize(serialize($b));
var_dump($c);

?>
--EXPECT--
object(a)#1 (1) {
  ["a"]=>
  int(4)
}
object(a)#2 (1) {
  ["a"]=>
  int(4)
}
