--TEST--
#[\DelayedTargetValidation] affects errors from validators
--FILE--
<?php

#[DelayedTargetValidation]
#[AllowDynamicProperties]
trait DemoTrait {}

#[DelayedTargetValidation]
#[AllowDynamicProperties]
interface DemoInterface {}

#[DelayedTargetValidation]
#[AllowDynamicProperties]
readonly class DemoReadonly {}

#[DelayedTargetValidation]
#[AllowDynamicProperties]
enum DemoEnum {}

class DemoClass {
	public string $hooked {
		#[DelayedTargetValidation]
		#[NoDiscard] // Does nothing here
		get => $this->hooked;
		#[DelayedTargetValidation]
		#[NoDiscard] // Does nothing here
		set => $value;
	}
}

$cases = [
	new ReflectionClass('DemoTrait'),
	new ReflectionClass('DemoInterface'),
	new ReflectionClass('DemoReadonly'),
	new ReflectionClass('DemoEnum'),
	new ReflectionProperty('DemoClass', 'hooked')->getHook(PropertyHookType::Get),
	new ReflectionProperty('DemoClass', 'hooked')->getHook(PropertyHookType::Set),
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
    string(22) "AllowDynamicProperties"
  }
}
Error: Cannot apply #[\AllowDynamicProperties] to trait DemoTrait
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
    string(22) "AllowDynamicProperties"
  }
}
Error: Cannot apply #[\AllowDynamicProperties] to interface DemoInterface
********************
Class [ <user> readonly class DemoReadonly ] {
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
Error: Cannot apply #[\AllowDynamicProperties] to readonly class DemoReadonly
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
    string(22) "AllowDynamicProperties"
  }
}
Error: Cannot apply #[\AllowDynamicProperties] to enum DemoEnum
********************
Method [ <user> public method $hooked::get ] {
  @@ %s %d - %d

  - Parameters [0] {
  }
  - Return [ string ]
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
    string(9) "NoDiscard"
  }
}
Error: #[\NoDiscard] is not supported for property hooks
********************
Method [ <user> public method $hooked::set ] {
  @@ %s %d - %d

  - Parameters [1] {
    Parameter #0 [ <required> string $value ]
  }
  - Return [ void ]
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
    string(9) "NoDiscard"
  }
}
Error: #[\NoDiscard] is not supported for property hooks
