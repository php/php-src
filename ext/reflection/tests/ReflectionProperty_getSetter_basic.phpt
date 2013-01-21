--TEST--
ReflectionProperty::getSet();
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
		var_dump($objProperty->getSet());
	}
?>
==DONE==
--EXPECT--
bool(false)
object(ReflectionMethod)#5 (2) {
  ["name"]=>
  string(8) "$a2->set"
  ["class"]=>
  string(10) "TimePeriod"
}
object(ReflectionMethod)#5 (2) {
  ["name"]=>
  string(8) "$a3->set"
  ["class"]=>
  string(10) "TimePeriod"
}
==DONE==
