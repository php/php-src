--TEST--
Constructor promotion can be used inside a trait
--FILE--
<?php

trait Test {
    public function __construct(public $prop) {}
}

class Test2 {
    use Test;
}

var_dump(new Test2(42));

?>
--EXPECT--
object(Test2)#1 (1) {
  ["prop"]=>
  int(42)
}
