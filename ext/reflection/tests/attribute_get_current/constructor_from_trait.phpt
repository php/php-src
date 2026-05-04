--TEST--
ReflectionAttribute::getCurrent() allowed when attribute constructor comes from trait
--FILE--
<?php

trait DemoTrait {
	public function __construct($args) {
		echo "In trait\n";
		echo __METHOD__ . "()\n";
		echo ReflectionAttribute::getCurrent();
	}
}

#[Attribute]
class Demo {
	use DemoTrait;
}

#[Demo("class")]
class WithDemo {

}

$case = new ReflectionClass(WithDemo::class);
echo $case;
echo "\n";
$case->getAttributes()[0]->newInstance();


?>
--EXPECTF--
Class [ <user> class WithDemo ] {
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

In trait
DemoTrait::__construct()
Class [ <user> class WithDemo ] {
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
