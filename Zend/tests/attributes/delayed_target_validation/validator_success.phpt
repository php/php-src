--TEST--
#[\DelayedTargetValidation] with a successful validator
--FILE--
<?php

#[DelayedTargetValidation]
#[AllowDynamicProperties]
class DemoClass {}

$obj = new DemoClass();
var_dump($obj);
// No warnings
$obj->dynamic = true;
var_dump($obj);

$ref = new ReflectionClass('DemoClass');
echo $ref . "\n";
$attributes = $ref->getAttributes();
var_dump($attributes);
var_dump($attributes[1]->newInstance());

?>
--EXPECTF--
object(DemoClass)#%d (0) {
}
object(DemoClass)#%d (1) {
  ["dynamic"]=>
  bool(true)
}
Class [ <user> class DemoClass ] {
  @@ %s %d-%d

  - Constants [0] {
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [0] {
  }

  - Methods [0] {
  }
}

array(2) {
  [0]=>
  object(ReflectionAttribute)#%d (1) {
    ["name"]=>
    string(23) "DelayedTargetValidation"
  }
  [1]=>
  object(ReflectionAttribute)#%d (1) {
    ["name"]=>
    string(22) "AllowDynamicProperties"
  }
}
object(AllowDynamicProperties)#%d (0) {
}
