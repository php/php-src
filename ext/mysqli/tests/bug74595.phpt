--TEST--
Bug #74595 (ReflectionMethod::getParameters returns incorrect number of parameters)
--SKIPIF--
<?php
require_once('skipif.inc');
?>
--FILE--
<?php
$class = new ReflectionClass('mysqli');
$method = $class->getMethod('query');
var_dump($method->getParameters());
?>
--EXPECTF--
array(2) {
  [0]=>
  object(ReflectionParameter)#%d (1) {
    ["name"]=>
    string(5) "query"
  }
  [1]=>
  object(ReflectionParameter)#%d (1) {
    ["name"]=>
    string(10) "resultmode"
  }
}
