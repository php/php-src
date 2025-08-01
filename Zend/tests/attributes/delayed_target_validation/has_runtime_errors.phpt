--TEST--
#[\DelayedTargetValidation] has errors at runtime
--FILE--
<?php

#[DelayedTargetValidation]
#[NoDiscard]
class Demo {

	#[DelayedTargetValidation]
	#[Attribute]
	public const FOO = 'BAR';

	#[DelayedTargetValidation]
	#[Attribute]
	public string $v1;

	public string $v2 {
		#[DelayedTargetValidation]
		#[Attribute]
		get => $this->v2;
		#[DelayedTargetValidation]
		#[Attribute]
		set => $value;
	}

	#[DelayedTargetValidation]
	#[Attribute]
	public function __construct(
		#[DelayedTargetValidation]
		#[Attribute]
		public string $v3
	) {
		$this->v1 = $v3;
		echo __METHOD__ . "\n";
	}
}

#[DelayedTargetValidation]
#[Attribute]
function demoFn() {
	echo __FUNCTION__ . "\n";
}

#[DelayedTargetValidation]
#[Attribute]
const EXAMPLE = true;

$cases = [
	new ReflectionClass('Demo'),
	new ReflectionClassConstant('Demo', 'FOO'),
	new ReflectionProperty('Demo', 'v1'),
	new ReflectionProperty('Demo', 'v2')->getHook(PropertyHookType::Get),
	new ReflectionProperty('Demo', 'v2')->getHook(PropertyHookType::Set),
	new ReflectionMethod('Demo', '__construct'),
	new ReflectionParameter([ 'Demo', '__construct' ], 'v3'),
	new ReflectionProperty('Demo', 'v3'),
	new ReflectionFunction('demoFn'),
	new ReflectionConstant('EXAMPLE'),
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
Class [ <user> <iterateable> class Demo ] {
  @@ %s %d-%d

  - Constants [1] {
    Constant [ public string FOO ] { BAR }
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [3] {
    Property [ public string $v1 ]
    Property [ public string $v2 { get; set; } ]
    Property [ public string $v3 ]
  }

  - Methods [1] {
    Method [ <user, ctor> public method __construct ] {
      @@ %s %d - %d

      - Parameters [1] {
        Parameter #0 [ <required> string $v3 ]
      }
    }
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
    string(9) "NoDiscard"
  }
}
Error: Attribute "NoDiscard" cannot target class (allowed targets: function, method)
********************
Constant [ public string FOO ] { BAR }

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
Error: Attribute "Attribute" cannot target class constant (allowed targets: class)
********************
Property [ public string $v1 ]

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
Error: Attribute "Attribute" cannot target property (allowed targets: class)
********************
Method [ <user> public method $v2::get ] {
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
    string(9) "Attribute"
  }
}
Error: Attribute "Attribute" cannot target method (allowed targets: class)
********************
Method [ <user> public method $v2::set ] {
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
    string(9) "Attribute"
  }
}
Error: Attribute "Attribute" cannot target method (allowed targets: class)
********************
Method [ <user, ctor> public method __construct ] {
  @@ %s %d - %d

  - Parameters [1] {
    Parameter #0 [ <required> string $v3 ]
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
Error: Attribute "Attribute" cannot target method (allowed targets: class)
********************
Parameter #0 [ <required> string $v3 ]
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
Error: Attribute "Attribute" cannot target parameter (allowed targets: class)
********************
Property [ public string $v3 ]

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
Error: Attribute "Attribute" cannot target property (allowed targets: class)
********************
Function [ <user> function demoFn ] {
  @@ %s %d - %d
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
Error: Attribute "Attribute" cannot target function (allowed targets: class)
********************
Constant [ bool EXAMPLE ] { 1 }

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
Error: Attribute "Attribute" cannot target constant (allowed targets: class)
