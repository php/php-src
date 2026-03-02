--TEST--
GH-20377: Constructor promotion with a final property without visibility set
--FILE--
<?php

class Demo {
    public function __construct(
        final string $foo,
        final public string $bar,
    ) {}
}

$d = new Demo("first", "second");
var_dump($d);

?>
--EXPECTF--
object(Demo)#%d (2) {
  ["foo"]=>
  string(5) "first"
  ["bar"]=>
  string(6) "second"
}
