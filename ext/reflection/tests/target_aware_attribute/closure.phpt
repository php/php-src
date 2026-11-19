--TEST--
ReflectionTargetAwareAttribute attribute for closure
--FILE--
<?php

#[Attribute]
class Demo implements ReflectionTargetAwareAttribute {

	public function setAttributeTarget(ReflectionAttributeTarget $target): void {
		echo $target;
	}
}

$closure = #[Demo("closure")] static function (mixed $param) {};

$case = new ReflectionFunction($closure);
echo $case;
echo "\n";
$case->getAttributes()[0]->newInstance();

?>
--EXPECTF--
Closure [ <user> static function {closure:%s:%d} ] {
  @@ %s %d - %d

  - Parameters [1] {
    Parameter #0 [ <required> mixed $param ]
  }
}

Closure [ <user> static function {closure:%s:%d} ] {
  @@ %s %d - %d

  - Parameters [1] {
    Parameter #0 [ <required> mixed $param ]
  }
}
