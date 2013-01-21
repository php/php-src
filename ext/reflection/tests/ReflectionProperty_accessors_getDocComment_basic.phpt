--TEST--
ReflectionPropertyAccessor::getDocComment();
--CREDITS--
Clint Priest <php-dev@zerocue.com>
--FILE--
<?php
	class TimePeriod {
		/** Test Comment for $a1 */
		public $a1 {
			/** Test Getter Comment for $a1 */
			get { return 1; }
		}
		
		/** Test Comment for $a2 */
		public  $a2 {
			set { }
		}
		/** Test Comment for $a3 */
		private $a3 {
			get { }
			set { }
		}
	}
	$rc = new ReflectionClass('TimePeriod');
	foreach($rc->getProperties() as $objProperty) {
		var_dump($objProperty->getName());
		var_dump($objProperty->getDocComment());
		if($objProperty->getGet()) {
			echo "getter DocComment\n";
			var_dump($objProperty->getGet()->getDocComment());
		}
		if($objProperty->getSet()) {
			echo "setter DocComment\n";
			var_dump($objProperty->getSet()->getDocComment());
		}
		echo "******************\n";
	}
?>
==DONE==
--EXPECTF--
string(2) "a1"
string(%d) "/** Test Comment for $a1 */"
getter DocComment
string(%d) "/** Test Getter Comment for $a1 */"
******************
string(2) "a2"
string(%d) "/** Test Comment for $a2 */"
setter DocComment
bool(false)
******************
string(2) "a3"
string(%d) "/** Test Comment for $a3 */"
getter DocComment
bool(false)
setter DocComment
bool(false)
******************
==DONE==
