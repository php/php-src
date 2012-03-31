--TEST--
ReflectionPropertyAccessor::getDocComment();
--CREDITS--
Clint Priest <php-dev@zerocue.com>
--FILE--
<?php
	class TimePeriod {
		/** Test Read-Only Comment for $a1 */
		public read-only $a1 {
			/** Test Getter Read-Only Comment for $a1 */
			get { return 1; }
		}
		
		/** Test Write-Only Comment for $a2 */
		public write-only $a2 {
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
		if($objProperty->getGetter()) {
			echo "getter DocComment\n";
			var_dump($objProperty->getGetter()->getDocComment());
		}
		if($objProperty->getSetter()) {
			echo "setter DocComment\n";
			var_dump($objProperty->getSetter()->getDocComment());
		}
		echo "******************\n";
	}
?>
==DONE==
--EXPECT--
string(2) "a1"
string(37) "/** Test Read-Only Comment for $a1 */"
getter DocComment
string(44) "/** Test Getter Read-Only Comment for $a1 */"
******************
string(2) "a2"
string(38) "/** Test Write-Only Comment for $a2 */"
setter DocComment
bool(false)
******************
string(2) "a3"
string(27) "/** Test Comment for $a3 */"
getter DocComment
bool(false)
setter DocComment
bool(false)
******************
==DONE==
