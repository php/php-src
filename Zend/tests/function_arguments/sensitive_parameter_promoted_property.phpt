--TEST--
The SensitiveParameter attribute works as well with promoted properties.
--FILE--
<?php

class WithSensitiveParameter
{
    public function __construct(#[SensitiveParameter] public string $password)
    {
        var_dump(debug_backtrace());
    }
}

new WithSensitiveParameter('sensitive');

?>
--EXPECTF--
array(1) {
  [0]=>
  array(7) {
    ["file"]=>
    string(%d) "%ssensitive_parameter_promoted_property.php"
    ["line"]=>
    int(11)
    ["function"]=>
    string(11) "__construct"
    ["class"]=>
    string(22) "WithSensitiveParameter"
    ["object"]=>
    object(WithSensitiveParameter)#1 (1) {
      ["password"]=>
      string(9) "sensitive"
    }
    ["type"]=>
    string(2) "->"
    ["args"]=>
    array(1) {
      [0]=>
      object(SensitiveParameterValue)#2 (0) {
      }
    }
  }
}
