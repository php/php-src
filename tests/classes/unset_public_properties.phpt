--TEST--
Un-setting public instance properties causes magic methods to be called when trying to access them from outside class scope
--FILE--
<?php

class Test
{
	public $testProperty = 'property set';
	
	public function __get($name)
	{
		return '__get ' . $name;
	}
	
	public function __set($name, $value)
	{
		$this->$name = $value;
		echo '__set ' . $name . ' to ' . $value;
	}
	
	public function __isset($name)
	{
		echo '__isset ' . $name;
		return isset($this->$name);
	}
	
	public function getTestProperty()
	{
		return $this->testProperty;
	}
	
	public function setTestProperty($testProperty)
	{
		$this->testProperty = $testProperty;
	}
}

$o = new Test;

echo $o->testProperty;
echo "\n";
isset($o->testProperty);
echo "\n";
unset($o->testProperty);
isset($o->testProperty);
echo "\n";
echo $o->testProperty;
echo "\n";
echo $o->getTestProperty();
echo "\n";
echo $o->setTestProperty('new value via setter');
echo "\n";
echo $o->testProperty;
echo "\n";
unset($o->testProperty);
$o->testProperty = 'new value via public access';
echo "\n";
isset($o->testProperty);
echo "\n";
echo $o->testProperty;

?>
--EXPECTF--
property set

__isset testProperty
__get testProperty
__get testProperty
__set testProperty to new value via setter
new value via setter
__set testProperty to new value via public access

new value via public access
