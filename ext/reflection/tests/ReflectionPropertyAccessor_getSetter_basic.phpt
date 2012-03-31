--TEST--
ReflectionPropertyAccessor::getSetter();
--CREDITS--
Clint Priest <php-dev@zerocue.com>
--FILE--
<?php
	class TimePeriod {
		public read-only $a1 {
			get { return 1; }
		}
		public write-only $a2 {
			set { }
		}
		private $a3 {
			get { }
			set { }
		}
	}
	$rc = new ReflectionClass('TimePeriod');
	foreach($rc->getProperties() as $objProperty) {
		var_dump($objProperty->getSetter());
	}
?>
==DONE==
--EXPECT--
bool(false)
object(ReflectionMethod)#5 (2) {
  ["name"]=>
  string(7) "__seta2"
  ["class"]=>
  string(10) "TimePeriod"
}
object(ReflectionMethod)#5 (2) {
  ["name"]=>
  string(7) "__seta3"
  ["class"]=>
  string(10) "TimePeriod"
}
==DONE==
