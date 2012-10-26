--TEST--
ZE2 Tests that a getter/setter can access parent class getter/setter
--FILE--
<?php

	class Shape {
		protected $_a = 16;
		
		public $Area {
			get { echo "Shape::getArea()\r\n"; return $this->_a; }
			set { echo "Shape::setArea($value)\r\n"; $this->_a = $value; }
		}
	}
	
	class Shape2 extends Shape {
		public $Area {
			get { echo "Shape2::getArea();\r\n"; return parent::$Area; }
			set { echo "Shape2::setArea($value);\r\n"; parent::$Area = $value; }
		}
	}
	
	$o = new Shape2();
	
	echo $o->Area."\r\n";
	$o->Area = 5;
	echo $o->Area."\r\n";

?>
--EXPECTF--
Shape2::getArea();
Shape::getArea()
16
Shape2::setArea(5);
Shape::setArea(5)
Shape2::getArea();
Shape::getArea()
5
