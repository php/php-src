--TEST--
ReflectionProperty::getUnset();
--CREDITS--
Clint Priest <php-dev@zerocue.com>
--FILE--
<?php
	class TimePeriod {
		public $a1 {
			get { return 1; }
		}
		public $a2 {
			set { }
		}
		private $a3 {
			get { }
			set { }
		}
	}
	$rc = new ReflectionClass('TimePeriod');
	foreach($rc->getProperties() as $objProperty) {
		var_dump($objProperty->getUnset());
	}
?>
==DONE==
--EXPECTF--
bool(false)
object(ReflectionMethod)#5 (2) {
  ["name"]=>
  string(%d) "$a2->unset"
  ["class"]=>
  string(10) "TimePeriod"
}
object(ReflectionMethod)#5 (2) {
  ["name"]=>
  string(%d) "$a3->unset"
  ["class"]=>
  string(10) "TimePeriod"
}
==DONE==
