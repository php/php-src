--TEST--
The SensitiveParameter attribute suppresses the single sensitive argument in a function called in eval().
--FILE--
<?php

function test(#[SensitiveParameter] $sensitive)
{
    debug_print_backtrace();
    var_dump(debug_backtrace());
    var_dump((new Exception)->getTrace());
}

eval(<<<'EOT'
test('sensitive');
EOT);

?>
--EXPECTF--
#0 %ssensitive_parameter_eval_call.php(11) : eval()'d code(1): test(Object(SensitiveParameterValue))
#1 %ssensitive_parameter_eval_call.php(11): eval()
array(2) {
  [0]=>
  array(4) {
    ["file"]=>
    string(%d) "%ssensitive_parameter_eval_call.php(11) : eval()'d code"
    ["line"]=>
    int(1)
    ["function"]=>
    string(4) "test"
    ["args"]=>
    array(1) {
      [0]=>
      object(SensitiveParameterValue)#%d (0) {
      }
    }
  }
  [1]=>
  array(3) {
    ["file"]=>
    string(%d) "%ssensitive_parameter_eval_call.php"
    ["line"]=>
    int(11)
    ["function"]=>
    string(4) "eval"
  }
}
array(2) {
  [0]=>
  array(4) {
    ["file"]=>
    string(%d) "%ssensitive_parameter_eval_call.php(11) : eval()'d code"
    ["line"]=>
    int(1)
    ["function"]=>
    string(4) "test"
    ["args"]=>
    array(1) {
      [0]=>
      object(SensitiveParameterValue)#%d (0) {
      }
    }
  }
  [1]=>
  array(3) {
    ["file"]=>
    string(%d) "%ssensitive_parameter_eval_call.php"
    ["line"]=>
    int(11)
    ["function"]=>
    string(4) "eval"
  }
}
