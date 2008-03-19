--TEST--
ZE2 ArrayAccess and ArrayProxyAccess, ArrayProxy
--FILE--
<?php 

// NOTE: This will become part of SPL

interface ArrayProxyAccess extends ArrayAccess
{
	function proxyGet($element);
	function proxySet($element, $index, $value);
	function proxyUnset($element, $index);
}

class ArrayProxy implements ArrayAccess
{
	private $object;
	private $element;
	
	function __construct(ArrayProxyAccess $object, $element)
	{
		echo __METHOD__ . "($element)\n";
		if (!$object->offsetExists($element))
		{
			$object[$element] = array();
		}
		$this->object = $object;
		$this->element = $element;
	}

	function offsetExists($index) {
		echo __METHOD__ . "($this->element, $index)\n";
		return array_key_exists($index, $this->object->proxyGet($this->element));
	}

	function offsetGet($index) {
		echo __METHOD__ . "($this->element, $index)\n";
		$tmp = $this->object->proxyGet($this->element);
		return isset($tmp[$index]) ? $tmp[$index] : NULL;
	}

	function offsetSet($index, $value) {
		echo __METHOD__ . "($this->element, $index, $value)\n";
		$this->object->proxySet($this->element, $index, $value);
	}

	function offsetUnset($index) {
		echo __METHOD__ . "($this->element, $index)\n";
		$this->object->proxyUnset($this->element, $index);
	}
}

class Peoples implements ArrayProxyAccess
{
	public $person;
	
	function __construct()
	{
		$this->person = array(array('name'=>'Foo'));
	}

	function offsetExists($index)
	{
		return array_key_exists($index, $this->person);
	}

	function offsetGet($index)
	{
		return new ArrayProxy($this, $index);
	}

	function offsetSet($index, $value)
	{
		$this->person[$index] = $value;
	}

	function offsetUnset($index)
	{
		unset($this->person[$index]);
	}

	function proxyGet($element)
	{
		return $this->person[$element];
	}

	function proxySet($element, $index, $value)
	{
		$this->person[$element][$index] = $value;
	}
	
	function proxyUnset($element, $index)
	{
		unset($this->person[$element][$index]);
	}
}

$people = new Peoples;

var_dump($people->person[0]['name']);
$people->person[0]['name'] = $people->person[0]['name'] . 'Bar';
var_dump($people->person[0]['name']);
$people->person[0]['name'] .= 'Baz';
var_dump($people->person[0]['name']);

echo "===ArrayOverloading===\n";

$people = new Peoples;

var_dump($people[0]);
var_dump($people[0]['name']);
$people[0]['name'] = 'FooBar';
var_dump($people[0]['name']);
$people[0]['name'] = $people->person[0]['name'] . 'Bar';
var_dump($people[0]['name']);
$people[0]['name'] .= 'Baz';
var_dump($people[0]['name']);
unset($people[0]['name']);
var_dump($people[0]);
var_dump($people[0]['name']);
$people[0]['name'] = 'BlaBla';
var_dump($people[0]['name']);

?>
===DONE===
--EXPECTF--
string(3) "Foo"
string(6) "FooBar"
string(9) "FooBarBaz"
===ArrayOverloading===
ArrayProxy::__construct(0)
object(ArrayProxy)#%d (2) {
  ["object:private"]=>
  object(Peoples)#%d (1) {
    ["person"]=>
    array(1) {
      [0]=>
      array(1) {
        ["name"]=>
        string(3) "Foo"
      }
    }
  }
  ["element:private"]=>
  int(0)
}
ArrayProxy::__construct(0)
ArrayProxy::offsetGet(0, name)
string(3) "Foo"
ArrayProxy::__construct(0)
ArrayProxy::offsetSet(0, name, FooBar)
ArrayProxy::__construct(0)
ArrayProxy::offsetGet(0, name)
string(6) "FooBar"
ArrayProxy::__construct(0)
ArrayProxy::offsetSet(0, name, FooBarBar)
ArrayProxy::__construct(0)
ArrayProxy::offsetGet(0, name)
string(9) "FooBarBar"
ArrayProxy::__construct(0)
ArrayProxy::offsetGet(0, name)
ArrayProxy::offsetSet(0, name, FooBarBarBaz)
ArrayProxy::__construct(0)
ArrayProxy::offsetGet(0, name)
string(12) "FooBarBarBaz"
ArrayProxy::__construct(0)
ArrayProxy::offsetUnset(0, name)
ArrayProxy::__construct(0)
object(ArrayProxy)#%d (2) {
  ["object:private"]=>
  object(Peoples)#%d (1) {
    ["person"]=>
    array(1) {
      [0]=>
      array(0) {
      }
    }
  }
  ["element:private"]=>
  int(0)
}
ArrayProxy::__construct(0)
ArrayProxy::offsetGet(0, name)
NULL
ArrayProxy::__construct(0)
ArrayProxy::offsetSet(0, name, BlaBla)
ArrayProxy::__construct(0)
ArrayProxy::offsetGet(0, name)
string(6) "BlaBla"
===DONE===
