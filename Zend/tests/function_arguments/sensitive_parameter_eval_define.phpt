--TEST--
The SensitiveParameter attribute suppresses the single sensitive argument in a function created in eval().
--FILE--
<?php

eval(<<<'EOT'
function test(#[SensitiveParameter] $sensitive)
{
    debug_print_backtrace();
    var_dump(debug_backtrace());
    var_dump((new Exception)->getTrace());
}
EOT);

test('sensitive');

?>
--EXPECTF--
#0 %ssensitive_parameter_eval_define.php(12): test(Object(SensitiveParameterValue))
array(1) {
  [0]=>
  array(4) {
    ["file"]=>
    string(%d) "%ssensitive_parameter_eval_define.php"
    ["line"]=>
    int(12)
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
    string(%d) "%ssensitive_parameter_eval_define.php"
    ["line"]=>
    int(12)
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
