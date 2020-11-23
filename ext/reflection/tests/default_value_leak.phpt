--TEST--
Check that isDefaultValueConstant() does not leak
--FILE--
<?php

class Test {
    public function method($param = [123]) {}
}

$rp = new ReflectionParameter(['Test', 'method'], 'param');
var_dump($rp->isDefaultValueAvailable());
var_dump($rp->isDefaultValueConstant());
var_dump($rp->getDefaultValue());

?>
--EXPECT--
bool(true)
bool(false)
array(1) {
  [0]=>
  int(123)
}
