<?

class BaseClass {
	var $class_name = "BaseClass";
	
	function BaseClass($value) {
		print "value is '$value'\n";
	}
	function MyClassName() {
		return $this->class_name;
	}
};


class ChildClass {
	var $class_name = "ChildClass";
	
	function ChildClass($value, $new_value) {
		BaseClass::BaseClass($value);
		print "new value is '$new_value'\n";
	}
	function MyClassName($a_value) {
		return BaseClass::MyClassName()." and the value is '$a_value'";
	}
};


$obj = new ChildClass("Test", "Another test");
print $obj->MyClassName("not interesting");