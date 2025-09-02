--TEST--
The SensitiveParameter attribute suppresses the single sensitive argument.
--FILE--
<?php

function test(#[SensitiveParameter] $sensitive)
{
    debug_print_backtrace();
    var_dump(debug_backtrace());
    var_dump((new Exception)->getTrace());
}

test('sensitive');

?>
--EXPECTF--
#0 %ssensitive_parameter.php(10): test(Object(SensitiveParameterValue))
array(1) {
  [0]=>
  array(4) {
    ["file"]=>
    string(%d) "%ssensitive_parameter.php"
    ["line"]=>
    int(10)
    ["function"]=>
    string(4) "test"
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
    string(%d) "%ssensitive_parameter.php"
    ["line"]=>
    int(10)
    ["function"]=>
    string(4) "test"
    ["args"]=>
    array(1) {
      [0]=>
      object(SensitiveParameterValue)#%d (0) {
      }
    }
  }
}
