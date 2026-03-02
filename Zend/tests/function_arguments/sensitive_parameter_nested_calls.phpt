--TEST--
The SensitiveParameter attribute handles nested function calls correctly.
--FILE--
<?php

function test(
    #[SensitiveParameter] $sensitive1 = null,
    $non_sensitive = null,
    #[SensitiveParameter] $sensitive2 = null,
)
{
    debug_print_backtrace();
    var_dump(debug_backtrace());
    var_dump((new Exception)->getTrace());
}

function wrapper(
    $non_sensitive = null,
    #[SensitiveParameter] $sensitive1 = null,
    #[SensitiveParameter] $sensitive2 = null,
)
{
    test($non_sensitive, $sensitive1, $sensitive2);
}

wrapper('foo', 'bar', 'baz');

?>
--EXPECTF--
#0 %ssensitive_parameter_nested_calls.php(20): test(Object(SensitiveParameterValue), 'bar', Object(SensitiveParameterValue))
#1 %ssensitive_parameter_nested_calls.php(23): wrapper('foo', Object(SensitiveParameterValue), Object(SensitiveParameterValue))
array(2) {
  [0]=>
  array(4) {
    ["file"]=>
    string(%d) "%ssensitive_parameter_nested_calls.php"
    ["line"]=>
    int(20)
    ["function"]=>
    string(4) "test"
    ["args"]=>
    array(3) {
      [0]=>
      object(SensitiveParameterValue)#%d (0) {
      }
      [1]=>
      string(3) "bar"
      [2]=>
      object(SensitiveParameterValue)#%d (0) {
      }
    }
  }
  [1]=>
  array(4) {
    ["file"]=>
    string(%d) "%ssensitive_parameter_nested_calls.php"
    ["line"]=>
    int(23)
    ["function"]=>
    string(7) "wrapper"
    ["args"]=>
    array(3) {
      [0]=>
      string(3) "foo"
      [1]=>
      object(SensitiveParameterValue)#%d (0) {
      }
      [2]=>
      object(SensitiveParameterValue)#%d (0) {
      }
    }
  }
}
array(2) {
  [0]=>
  array(4) {
    ["file"]=>
    string(%d) "%ssensitive_parameter_nested_calls.php"
    ["line"]=>
    int(20)
    ["function"]=>
    string(4) "test"
    ["args"]=>
    array(3) {
      [0]=>
      object(SensitiveParameterValue)#%d (0) {
      }
      [1]=>
      string(3) "bar"
      [2]=>
      object(SensitiveParameterValue)#%d (0) {
      }
    }
  }
  [1]=>
  array(4) {
    ["file"]=>
    string(%d) "%ssensitive_parameter_nested_calls.php"
    ["line"]=>
    int(23)
    ["function"]=>
    string(7) "wrapper"
    ["args"]=>
    array(3) {
      [0]=>
      string(3) "foo"
      [1]=>
      object(SensitiveParameterValue)#%d (0) {
      }
      [2]=>
      object(SensitiveParameterValue)#%d (0) {
      }
    }
  }
}
