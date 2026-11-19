--TEST--
ReflectionTargetAwareAttribute attribute for function
--FILE--
<?php

#[Attribute]
class Demo implements ReflectionTargetAwareAttribute {

	public function setAttributeTarget(ReflectionAttributeTarget $target): void {
		echo $target;
	}
}

#[Demo("function")]
function globalFunc(
	mixed $param
) {}

$case = new ReflectionFunction('globalFunc');
echo $case;
echo "\n";
$case->getAttributes()[0]->newInstance();

?>
--EXPECTF--
Function [ <user> function globalFunc ] {
  @@ %s %d - %d

  - Parameters [1] {
    Parameter #0 [ <required> mixed $param ]
  }
}

Function [ <user> function globalFunc ] {
  @@ %s %d - %d

  - Parameters [1] {
    Parameter #0 [ <required> mixed $param ]
  }
}
