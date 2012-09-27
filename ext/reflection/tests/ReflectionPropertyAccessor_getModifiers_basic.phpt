--TEST--
ReflectionPropertyAccessor::getModifiers();
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
		echo $objProperty->name.' = ';
		var_dump($objProperty->getModifiers());
	}
?>
==DONE==
--EXPECT--
a1 = int(536871168)
a2 = int(1073742080)
a3 = int(1024)
==DONE==
