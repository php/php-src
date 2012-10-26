--TEST--
ZE2 Tests that a static getter/setter function properly
--FILE--
<?php

	class Shape {
		private static $_a = 5;
		
		public static $Area {
			get { echo "Shape::getArea()\r\n"; return self::$_a; }
			set { echo "Shape::setArea($value)\r\n"; self::$_a = $value; }
		}
	}
			
	echo Shape::$Area."\r\n";
	Shape::$Area = 25;
	echo Shape::$Area."\r\n";

?>
--EXPECTF--
Shape::getArea()
5
Shape::setArea(25)
Shape::getArea()
25
