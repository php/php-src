--TEST--
Random: Engine: __serialize() must not expose INDIRECTs
--FILE--
<?php

$randomizer = new Random\Randomizer(null);
var_dump($randomizer->__serialize());

?>
--EXPECT--
array(1) {
  [0]=>
  array(1) {
    ["engine"]=>
    object(Random\Engine\Secure)#2 (0) {
    }
  }
}
