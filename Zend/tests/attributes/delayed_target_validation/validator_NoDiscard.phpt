--TEST--
#[\DelayedTargetValidation] with #[\NoDiscard]: validator errors delayed
--FILE--
<?php

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
