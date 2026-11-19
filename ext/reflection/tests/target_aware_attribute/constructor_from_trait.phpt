--TEST--
setAttributeTarget() implementation comes from trait
--FILE--
<?php

trait DemoTrait {
	public function setAttributeTarget(ReflectionAttributeTarget $target): void {
    echo "In trait\n";
    echo __METHOD__ . "()\n";
		echo $target;
	}
}

#[Attribute]
class Demo implements ReflectionTargetAwareAttribute {
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
DemoTrait::setAttributeTarget()
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
