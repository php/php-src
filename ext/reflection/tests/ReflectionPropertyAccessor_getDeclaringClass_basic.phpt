--TEST--
ReflectionPropertyAccessor::getDeclaringClass();
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
		echo $objProperty->name.' = ';
		var_dump($objProperty->getDeclaringClass());
	}
?>
==DONE==
--EXPECT--
a1 = object(ReflectionClass)#5 (1) {
  ["name"]=>
  string(10) "TimePeriod"
}
a2 = object(ReflectionClass)#5 (1) {
  ["name"]=>
  string(10) "TimePeriod"
}
a3 = object(ReflectionClass)#5 (1) {
  ["name"]=>
  string(10) "TimePeriod"
}
==DONE==
