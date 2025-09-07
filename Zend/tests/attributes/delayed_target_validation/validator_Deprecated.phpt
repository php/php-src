--TEST--
#[\DelayedTargetValidation] with #[\Deprecated]: validator errors delayed
--FILE--
<?php

#[DelayedTargetValidation]
#[Deprecated]
interface DemoInterface {}

#[DelayedTargetValidation]
#[Deprecated]
class DemoClass {}

#[DelayedTargetValidation]
#[Deprecated]
enum DemoEnum {}

$cases = [
	new ReflectionClass('DemoInterface'),
	new ReflectionClass('DemoClass'),
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
    string(10) "Deprecated"
  }
}
Error: Cannot apply #[\Deprecated] to interface DemoInterface
********************
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
    string(10) "Deprecated"
  }
}
Error: Cannot apply #[\Deprecated] to class DemoClass
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
    string(10) "Deprecated"
  }
}
Error: Cannot apply #[\Deprecated] to enum DemoEnum
