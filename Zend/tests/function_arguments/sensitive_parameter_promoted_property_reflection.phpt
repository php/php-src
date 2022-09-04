--TEST--
The SensitiveParameter attribute can not be instantiated against property
--FILE--
<?php

class WithSensitiveParameter
{
    public function __construct(#[SensitiveParameter] public string $password)
    {
    }
}
$reflection = (new ReflectionClass(WithSensitiveParameter::class));

var_dump(
    [
        'parameterAttribute' => $reflection->getConstructor()->getParameters()[0]->getAttributes()[0]->newInstance(),
        'propertyAttribute' => $reflection->getProperties()[0]->getAttributes(),
    ]
);

?>
--EXPECTF--
array(2) {
  ["parameterAttribute"]=>
  object(SensitiveParameter)#3 (0) {
  }
  ["propertyAttribute"]=>
  array(0) {
  }
}
