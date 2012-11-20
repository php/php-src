--TEST--
ReflectionPropertyAccessor::getModifiers();
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
		var_dump($objProperty->getModifiers());
	}
?>
==DONE==
--EXPECT--
a1 = int(256)
a2 = int(256)
a3 = int(1024)
==DONE==
