--TEST--
#[\DelayedTargetValidation] with #[\Attribute]: validator errors delayed
--FILE--
<?php

#[DelayedTargetValidation]
#[Attribute]
trait DemoTrait {}

#[DelayedTargetValidation]
#[Attribute]
interface DemoInterface {}

#[DelayedTargetValidation]
#[Attribute]
abstract class DemoAbstract {}

#[DelayedTargetValidation]
#[Attribute]
enum DemoEnum {}

$cases = [
	new ReflectionClass('DemoTrait'),
	new ReflectionClass('DemoInterface'),
	new ReflectionClass('DemoAbstract'),
	new ReflectionClass('DemoEnum'),
];
foreach ($cases as $r) {
	echo str_repeat("*", 20) . "\n";
	echo $r . "\n";
	$attributes = $r->getAttributes();
	var_dump($attributes);
	try {
		$attributes[1]->newInstance();
	} catch (Error $e) {
		echo get_class($e) . ": " . $e->getMessage() . "\n";
	}
}

?>
--EXPECTF--
********************
Trait [ <user> trait DemoTrait ] {
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
    string(9) "Attribute"
  }
}
Error: Cannot apply #[\Attribute] to trait DemoTrait
********************
Interface [ <user> interface DemoInterface ] {
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
    string(9) "Attribute"
  }
}
Error: Cannot apply #[\Attribute] to interface DemoInterface
********************
Class [ <user> abstract class DemoAbstract ] {
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
    string(9) "Attribute"
  }
}
Error: Cannot apply #[\Attribute] to abstract class DemoAbstract
********************
Enum [ <user> enum DemoEnum implements UnitEnum ] {
  @@ %s %d-%d

  - Constants [0] {
  }

  - Static properties [0] {
  }

  - Static methods [1] {
    Method [ <internal, prototype UnitEnum> static public method cases ] {

      - Parameters [0] {
      }
      - Return [ array ]
    }
  }

  - Properties [1] {
    Property [ public protected(set) readonly string $name ]
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
    string(9) "Attribute"
  }
}
Error: Cannot apply #[\Attribute] to enum DemoEnum
