--TEST--
Bug #60201 (SplFileObject::setCsvControl does not expose third argument via Reflection)
--FILE--
<?php

$method = new ReflectionMethod('SplFileObject', 'setCsvControl');
$params = $method->getParameters(); 
var_dump($params);

?>
===DONE===
--EXPECTF--
array(3) {
  [0]=>
  object(ReflectionParameter)#%d (1) {
    ["name"]=>
    string(9) "delimiter"
  }
  [1]=>
  object(ReflectionParameter)#%d (1) {
    ["name"]=>
    string(9) "enclosure"
  }
  [2]=>
  object(ReflectionParameter)#%d (1) {
    ["name"]=>
    string(6) "escape"
  }
}
===DONE===
