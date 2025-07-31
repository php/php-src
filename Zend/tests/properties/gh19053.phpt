--TEST--
GH-19053: Incorrect properties_info_table for abstract properties
--FILE--
<?php

abstract class GP {
    public abstract mixed $foo { get; }
}

class P extends GP {
    public mixed $foo = 1;
}

class C extends P {
    public mixed $foo { get => 2; }
}

$c = new C;
var_dump($c);

?>
--EXPECTF--
object(C)#%d (0) {
  ["foo"]=>
  uninitialized(mixed)
}
