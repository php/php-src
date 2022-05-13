--TEST--
The SensitiveParameter attribute suppresses the single sensitive argument for closures.
--FILE--
<?php

$test = function (#[SensitiveParameter] $sensitive)
{
    debug_print_backtrace();
    var_dump(debug_backtrace());
    var_dump((new Exception)->getTrace());
};

$test('sensitive');

?>
--EXPECTF--
#0 %ssensitive_parameter_closure.php(10): {closure}(Object(SensitiveParameterValue))
array(1) {
  [0]=>
  array(4) {
    ["file"]=>
    string(%d) "%ssensitive_parameter_closure.php"
    ["line"]=>
    int(10)
    ["function"]=>
    string(9) "{closure}"
    ["args"]=>
    array(1) {
      [0]=>
      object(SensitiveParameterValue)#%d (0) {
      }
    }
  }
}
array(1) {
  [0]=>
  array(4) {
    ["file"]=>
    string(%d) "%ssensitive_parameter_closure.php"
    ["line"]=>
    int(10)
    ["function"]=>
    string(9) "{closure}"
    ["args"]=>
    array(1) {
      [0]=>
      object(SensitiveParameterValue)#%d (0) {
      }
    }
  }
}
