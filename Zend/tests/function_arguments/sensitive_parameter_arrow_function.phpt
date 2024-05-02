--TEST--
The SensitiveParameter attribute suppresses the single sensitive argument for arrow functions.
--FILE--
<?php

$test = fn (#[SensitiveParameter] $sensitive) => (new Exception)->getTrace();

var_dump($test('sensitive'));

?>
--EXPECTF--
array(1) {
  [0]=>
  array(4) {
    ["file"]=>
    string(%d) "%ssensitive_parameter_arrow_function.php"
    ["line"]=>
    int(5)
    ["function"]=>
    string(%d) "{closure:%s:%d}"
    ["args"]=>
    array(1) {
      [0]=>
      object(SensitiveParameterValue)#%d (0) {
      }
    }
  }
}
