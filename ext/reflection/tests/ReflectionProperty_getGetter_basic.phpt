--TEST--
ReflectionProperty::getGet();
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
		var_dump($objProperty->getGet());
	}
?>
==DONE==
--EXPECT--
object(ReflectionMethod)#5 (2) {
  ["name"]=>
  string(8) "$a1->get"
  ["class"]=>
  string(10) "TimePeriod"
}
bool(false)
object(ReflectionMethod)#5 (2) {
  ["name"]=>
  string(8) "$a3->get"
  ["class"]=>
  string(10) "TimePeriod"
}
==DONE==
