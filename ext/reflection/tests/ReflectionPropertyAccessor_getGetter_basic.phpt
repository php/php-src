--TEST--
ReflectionPropertyAccessor::getGetter();
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
		var_dump($objProperty->getGetter());
	}
?>
==DONE==
--EXPECT--
object(ReflectionMethod)#5 (2) {
  ["name"]=>
  string(7) "__geta1"
  ["class"]=>
  string(10) "TimePeriod"
}
bool(false)
object(ReflectionMethod)#5 (2) {
  ["name"]=>
  string(7) "__geta3"
  ["class"]=>
  string(10) "TimePeriod"
}
==DONE==
