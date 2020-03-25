--TEST--
Dumping of uninitialized typed properties (including private ones)
--FILE--
<?php

class Test {
    public bool $public;
    protected float $protected;
    private string $private;

    public function dump() {
        var_dump($this);
        debug_zval_dump($this);
    }
}

$test = new Test;
$test->dump();

?>
--EXPECTF--
object(Test)#1 (0) {
  ["public"]=>
  uninitialized(bool)
  ["protected":protected]=>
  uninitialized(float)
  ["private":"Test":private]=>
  uninitialized(string)
}
object(Test)#1 (0) refcount(%d){
  ["public"]=>
  uninitialized(bool)
  ["protected":protected]=>
  uninitialized(float)
  ["private":"Test":private]=>
  uninitialized(string)
}
