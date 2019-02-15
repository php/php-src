--TEST--
Bug #68479 (Escape parameter missing from SplFileObject::fputcsv)
--FILE--
<?php

$method = new ReflectionMethod('SplFileObject', 'fputcsv');
$params = $method->getParameters();
var_dump($params);

?>
===DONE===
--EXPECT--
array(4) {
  [0]=>
  object(ReflectionParameter)#2 (1) {
    ["name"]=>
    string(6) "fields"
  }
  [1]=>
  object(ReflectionParameter)#3 (1) {
    ["name"]=>
    string(9) "delimiter"
  }
  [2]=>
  object(ReflectionParameter)#4 (1) {
    ["name"]=>
    string(9) "enclosure"
  }
  [3]=>
  object(ReflectionParameter)#5 (1) {
    ["name"]=>
    string(6) "escape"
  }
}
===DONE===
