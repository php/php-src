--TEST--
The SensitiveParameter attribute suppresses all variadic arguments.
--FILE--
<?php

function test(
    #[SensitiveParameter] ...$args
)
{
    debug_print_backtrace();
    var_dump(debug_backtrace());
    var_dump((new Exception)->getTrace());
}

test('foo', 'bar', 'baz');

?>
--EXPECTF--
#0 %ssensitive_parameter_variadic_arguments.php(12): test(Object(SensitiveParameterValue), Object(SensitiveParameterValue), Object(SensitiveParameterValue))
array(1) {
  [0]=>
  array(4) {
    ["file"]=>
    string(%d) "%ssensitive_parameter_variadic_arguments.php"
    ["line"]=>
    int(12)
    ["function"]=>
    string(4) "test"
    ["args"]=>
    array(3) {
      [0]=>
      object(SensitiveParameterValue)#%d (0) {
      }
      [1]=>
      object(SensitiveParameterValue)#%d (0) {
      }
      [2]=>
      object(SensitiveParameterValue)#%d (0) {
      }
    }
  }
}
array(1) {
  [0]=>
  array(4) {
    ["file"]=>
    string(%d) "%ssensitive_parameter_variadic_arguments.php"
    ["line"]=>
    int(12)
    ["function"]=>
    string(4) "test"
    ["args"]=>
    array(3) {
      [0]=>
      object(SensitiveParameterValue)#%d (0) {
      }
      [1]=>
      object(SensitiveParameterValue)#%d (0) {
      }
      [2]=>
      object(SensitiveParameterValue)#%d (0) {
      }
    }
  }
}
