--TEST--
The SensitiveParameter attribute handles named arguments.
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

test(non_sensitive: 'non_sensitive', sensitive2: 'sensitive2');

?>
--EXPECTF--
#0 %ssensitive_parameter_named_arguments.php(14): test(Object(SensitiveParameterValue), 'non_sensitive', Object(SensitiveParameterValue))
array(1) {
  [0]=>
  array(4) {
    ["file"]=>
    string(%d) "%ssensitive_parameter_named_arguments.php"
    ["line"]=>
    int(14)
    ["function"]=>
    string(4) "test"
    ["args"]=>
    array(3) {
      [0]=>
      object(SensitiveParameterValue)#%d (0) {
      }
      [1]=>
      string(13) "non_sensitive"
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
    string(%d) "%ssensitive_parameter_named_arguments.php"
    ["line"]=>
    int(14)
    ["function"]=>
    string(4) "test"
    ["args"]=>
    array(3) {
      [0]=>
      object(SensitiveParameterValue)#%d (0) {
      }
      [1]=>
      string(13) "non_sensitive"
      [2]=>
      object(SensitiveParameterValue)#%d (0) {
      }
    }
  }
}
