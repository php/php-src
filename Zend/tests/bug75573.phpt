--TEST--
Bug #75573 (Segmentation fault in 7.1.12 and 7.0.26)
--FILE--
<?php

class A
{
	var $_stdObject;
	function initialize($properties = FALSE) {
		$this->_stdObject = $properties ? (object) $properties : new stdClass();
		parent::initialize();
	}
	function &__get($property)
	{
		if (isset($this->_stdObject->{$property})) {
			$retval =& $this->_stdObject->{$property};
			return $retval;
		} else {
			return NULL;
		}
	}
	function &__set($property, $value)
	{
		return $this->_stdObject->{$property} = $value;
	}
	function __isset($property_name)
	{
		return isset($this->_stdObject->{$property_name});
	}
}

class B extends A
{
	function initialize($properties = array())
	{
		parent::initialize($properties);
	}
	function &__get($property)
	{
		if (isset($this->settings) && isset($this->settings[$property])) {
			$retval =& $this->settings[$property];
			return $retval;
		} else {
			return parent::__get($property);
		}
	}
}

$b = new B();
$b->settings = [ "foo" => "bar", "name" => "abc" ];
var_dump($b->name);
var_dump($b->settings);
?>
--EXPECTF--
Warning: Creating default object from empty value in %sbug75573.php on line %d

Notice: Only variable references should be returned by reference in %sbug75573.php on line %d
string(3) "abc"
array(2) {
  ["foo"]=>
  string(3) "bar"
  ["name"]=>
  string(3) "abc"
}
