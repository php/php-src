--TEST--
ZE2 Tests that a static getter/setter function properly through parent accessors
--FILE--
<?php

	class Shape {
		private static $_a = 5;
		
		public static $Area {
			get { echo "Static Shape::getArea()\r\n"; return self::$_a; }
			set { echo "Static Shape::setArea($value)\r\n"; self::$_a = $value; }
		}
	}
	
	class Shape2 extends Shape {
		public static $Area {
			get { echo "Static Shape2::getArea()\r\n"; return parent::$Area; }
			set { echo "Static Shape2::setArea($value)\r\n"; parent::$Area = $value; }
		}
	}
	
	echo Shape2::$Area."\r\n";
	Shape2::$Area = 25;
	echo Shape2::$Area."\r\n";

	
	
/*********************************************************************************
 * Static accessors as a feature for first release was shelved, the lines below 
 *	are what this test should output when static accessors are written:
 *********************************************************************************
Static Shape2::getArea()
Static Shape::getArea()
5
Static Shape2::setArea(25)
Static Shape::setArea(25)
Static Shape2::getArea()
Static Shape::getArea()
25
 */


?>
--EXPECTF--
Fatal error: Cannot define static accessor %s, not supported at this time in %s on line %d