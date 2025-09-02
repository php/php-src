--TEST--
The SensitiveParameter attribute does not suppress superfluous arguments if the last parameter is sensitive.
--FILE--
<?php

function test(
    $non_sensitive,
    #[SensitiveParameter] $sensitive,
)
{
    debug_print_backtrace();
    var_dump(debug_backtrace());
    var_dump((new Exception)->getTrace());
}

test('foo', 'bar', 'baz');

?>
--EXPECTF--
#0 %ssensitive_parameter_extra_arguments.php(13): test('foo', Object(SensitiveParameterValue), 'baz')
array(1) {
  [0]=>
  array(4) {
    ["file"]=>
    string(%d) "%ssensitive_parameter_extra_arguments.php"
    ["line"]=>
    int(13)
    ["function"]=>
    string(4) "test"
    ["args"]=>
    array(3) {
      [0]=>
      string(3) "foo"
      [1]=>
      object(SensitiveParameterValue)#%d (0) {
      }
      [2]=>
      string(3) "baz"
    }
  }
}
array(1) {
  [0]=>
  array(4) {
    ["file"]=>
    string(%d) "%ssensitive_parameter_extra_arguments.php"
    ["line"]=>
    int(13)
    ["function"]=>
    string(4) "test"
    ["args"]=>
    array(3) {
      [0]=>
      string(3) "foo"
      [1]=>
      object(SensitiveParameterValue)#%d (0) {
      }
      [2]=>
      string(3) "baz"
    }
  }
}
