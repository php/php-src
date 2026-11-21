--TEST--
ReflectionTargetAwareAttribute attribute for class
--FILE--
<?php

#[Attribute]
class Demo implements ReflectionTargetAwareAttribute {

	public function setAttributeTarget(ReflectionAttributeTarget $target): void {
		echo $target;
	}
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
