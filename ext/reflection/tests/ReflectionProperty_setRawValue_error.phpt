--TEST--
Test ReflectionProperty::setRawValue() error cases.
--FILE--
<?php

class Example {
    public mixed $nonHooked;

    public mixed $hooked {
        set(mixed $value) {
            throw new Exception("hook should not be called");
            $this->hooked = "Not virtual";
        }
    }
}

#[AllowDynamicProperties]
class AnotherClass {
}

$hookedProp = new ReflectionProperty(Example::class, 'hooked');
$nonHookedProp = new ReflectionProperty(Example::class, 'nonHooked');

$instance = new Example();
$hookedProp->setRawValue($instance, "value1");
$nonHookedProp->setRawValue($instance, "value2");
var_dump($instance);

$other = new AnotherClass();
$hookedProp->setRawValue($other, "value1");
$nonHookedProp->setRawValue($other, "value2");
var_dump($other);
?>
--EXPECTF--
object(Example)#%d (2) {
  ["nonHooked"]=>
  string(6) "value2"
  ["hooked"]=>
  string(6) "value1"
}

Deprecated: Calling ReflectionProperty::setRawValue() with a given object that is not an instance of the class this property was declared in is deprecated in %s on line %d

Deprecated: Calling ReflectionProperty::setRawValue() with a given object that is not an instance of the class this property was declared in is deprecated in %s on line %d
object(AnotherClass)#%d (2) {
  ["hooked"]=>
  string(6) "value1"
  ["nonHooked"]=>
  string(6) "value2"
}
