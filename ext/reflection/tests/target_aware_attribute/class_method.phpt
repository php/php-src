--TEST--
ReflectionTargetAwareAttribute attribute for method
--FILE--
<?php

#[Attribute]
class Demo implements ReflectionTargetAwareAttribute {

	public function setAttributeTarget(ReflectionAttributeTarget $target): void {
		echo $target;
	}
}

class WithDemo {

	#[Demo("method")]
	public function method(
		mixed $param
	) {}
}

$case = new ReflectionMethod(WithDemo::class, 'method');
echo $case;
echo "\n";
$case->getAttributes()[0]->newInstance();

?>
--EXPECTF--
Method [ <user> public method method ] {
  @@ %s %d - %d

  - Parameters [1] {
    Parameter #0 [ <required> mixed $param ]
  }
}

Method [ <user> public method method ] {
  @@ %s %d - %d

  - Parameters [1] {
    Parameter #0 [ <required> mixed $param ]
  }
}
