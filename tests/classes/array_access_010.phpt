--TEST--
ZE2 ArrayAccess and ArrayReferenceProxy with references
--FILE--
<?php 

// NOTE: This will become part of SPL

class ArrayReferenceProxy implements ArrayAccess
{
	private $object;
	private $element;
	
	function __construct(ArrayAccess $object, array &$element)
	{
		echo __METHOD__ . "($element)\n";
		$this->object = $object;
		$this->element = &$element;
	}

	function offsetExists($index) {
		echo __METHOD__ . "($this->element, $index)\n";
		return array_key_exists($index, $this->element);
	}

	function offsetGet($index) {
		echo __METHOD__ . "($this->element, $index)\n";
		return isset($this->element[$index]) ? $this->element[$index] : NULL;
	}

	function offsetSet($index, $value) {
		echo __METHOD__ . "($this->element, $index, $value)\n";
		$this->element[$index] = $value;
	}

	function offsetUnset($index) {
		echo __METHOD__ . "($this->element, $index)\n";
		unset($this->element[$index]);
	}
}

class Peoples implements ArrayAccess
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
		return new ArrayReferenceProxy($this, $this->person[$index]);
	}

	function offsetSet($index, $value)
	{
		$this->person[$index] = $value;
	}

	function offsetUnset($index)
	{
		unset($this->person[$index]);
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
<?php exit(0); ?>
--EXPECTF--
string(3) "Foo"
string(6) "FooBar"
string(9) "FooBarBaz"
===ArrayOverloading===

Notice: Array to string conversion in %s on line %d
ArrayReferenceProxy::__construct(Array)
object(ArrayReferenceProxy)#%d (2) {
  ["object":"ArrayReferenceProxy":private]=>
  object(Peoples)#%d (1) {
    ["person"]=>
    array(1) {
      [0]=>
      &array(1) {
        ["name"]=>
        string(3) "Foo"
      }
    }
  }
  ["element":"ArrayReferenceProxy":private]=>
  &array(1) {
    ["name"]=>
    string(3) "Foo"
  }
}

Notice: Array to string conversion in %s on line %d
ArrayReferenceProxy::__construct(Array)

Notice: Array to string conversion in %s on line %d
ArrayReferenceProxy::offsetGet(Array, name)
string(3) "Foo"

Notice: Array to string conversion in %s on line %d
ArrayReferenceProxy::__construct(Array)

Notice: Array to string conversion in %s on line %d
ArrayReferenceProxy::offsetSet(Array, name, FooBar)

Notice: Array to string conversion in %s on line %d
ArrayReferenceProxy::__construct(Array)

Notice: Array to string conversion in %s on line %d
ArrayReferenceProxy::offsetGet(Array, name)
string(6) "FooBar"

Notice: Array to string conversion in %s on line %d
ArrayReferenceProxy::__construct(Array)

Notice: Array to string conversion in %s on line %d
ArrayReferenceProxy::offsetSet(Array, name, FooBarBar)

Notice: Array to string conversion in %s on line %d
ArrayReferenceProxy::__construct(Array)

Notice: Array to string conversion in %s on line %d
ArrayReferenceProxy::offsetGet(Array, name)
string(9) "FooBarBar"

Notice: Array to string conversion in %s on line %d
ArrayReferenceProxy::__construct(Array)

Notice: Array to string conversion in %s on line %d
ArrayReferenceProxy::offsetGet(Array, name)

Notice: Array to string conversion in %s on line %d
ArrayReferenceProxy::offsetSet(Array, name, FooBarBarBaz)

Notice: Array to string conversion in %s on line %d
ArrayReferenceProxy::__construct(Array)

Notice: Array to string conversion in %s on line %d
ArrayReferenceProxy::offsetGet(Array, name)
string(12) "FooBarBarBaz"

Notice: Array to string conversion in %s on line %d
ArrayReferenceProxy::__construct(Array)

Notice: Array to string conversion in %s on line %d
ArrayReferenceProxy::offsetUnset(Array, name)

Notice: Array to string conversion in %s on line %d
ArrayReferenceProxy::__construct(Array)
object(ArrayReferenceProxy)#%d (2) {
  ["object":"ArrayReferenceProxy":private]=>
  object(Peoples)#%d (1) {
    ["person"]=>
    array(1) {
      [0]=>
      &array(0) {
      }
    }
  }
  ["element":"ArrayReferenceProxy":private]=>
  &array(0) {
  }
}

Notice: Array to string conversion in %s on line %d
ArrayReferenceProxy::__construct(Array)

Notice: Array to string conversion in %s on line %d
ArrayReferenceProxy::offsetGet(Array, name)
NULL

Notice: Array to string conversion in %s on line %d
ArrayReferenceProxy::__construct(Array)

Notice: Array to string conversion in %s on line %d
ArrayReferenceProxy::offsetSet(Array, name, BlaBla)

Notice: Array to string conversion in %s on line %d
ArrayReferenceProxy::__construct(Array)

Notice: Array to string conversion in %s on line %d
ArrayReferenceProxy::offsetGet(Array, name)
string(6) "BlaBla"
===DONE===
--UEXPECTF--
unicode(3) "Foo"
unicode(6) "FooBar"
unicode(9) "FooBarBaz"
===ArrayOverloading===

Notice: Array to string conversion in %s on line %d
ArrayReferenceProxy::__construct(Array)
object(ArrayReferenceProxy)#%d (2) {
  [u"object":u"ArrayReferenceProxy":private]=>
  object(Peoples)#%d (1) {
    [u"person"]=>
    array(1) {
      [0]=>
      &array(1) {
        [u"name"]=>
        unicode(3) "Foo"
      }
    }
  }
  [u"element":u"ArrayReferenceProxy":private]=>
  &array(1) {
    [u"name"]=>
    unicode(3) "Foo"
  }
}

Notice: Array to string conversion in %s on line %d
ArrayReferenceProxy::__construct(Array)

Notice: Array to string conversion in %s on line %d
ArrayReferenceProxy::offsetGet(Array, name)
unicode(3) "Foo"

Notice: Array to string conversion in %s on line %d
ArrayReferenceProxy::__construct(Array)

Notice: Array to string conversion in %s on line %d
ArrayReferenceProxy::offsetSet(Array, name, FooBar)

Notice: Array to string conversion in %s on line %d
ArrayReferenceProxy::__construct(Array)

Notice: Array to string conversion in %s on line %d
ArrayReferenceProxy::offsetGet(Array, name)
unicode(6) "FooBar"

Notice: Array to string conversion in %s on line %d
ArrayReferenceProxy::__construct(Array)

Notice: Array to string conversion in %s on line %d
ArrayReferenceProxy::offsetSet(Array, name, FooBarBar)

Notice: Array to string conversion in %s on line %d
ArrayReferenceProxy::__construct(Array)

Notice: Array to string conversion in %s on line %d
ArrayReferenceProxy::offsetGet(Array, name)
unicode(9) "FooBarBar"

Notice: Array to string conversion in %s on line %d
ArrayReferenceProxy::__construct(Array)

Notice: Array to string conversion in %s on line %d
ArrayReferenceProxy::offsetGet(Array, name)

Notice: Array to string conversion in %s on line %d
ArrayReferenceProxy::offsetSet(Array, name, FooBarBarBaz)

Notice: Array to string conversion in %s on line %d
ArrayReferenceProxy::__construct(Array)

Notice: Array to string conversion in %s on line %d
ArrayReferenceProxy::offsetGet(Array, name)
unicode(12) "FooBarBarBaz"

Notice: Array to string conversion in %s on line %d
ArrayReferenceProxy::__construct(Array)

Notice: Array to string conversion in %s on line %d
ArrayReferenceProxy::offsetUnset(Array, name)

Notice: Array to string conversion in %s on line %d
ArrayReferenceProxy::__construct(Array)
object(ArrayReferenceProxy)#%d (2) {
  [u"object":u"ArrayReferenceProxy":private]=>
  object(Peoples)#%d (1) {
    [u"person"]=>
    array(1) {
      [0]=>
      &array(0) {
      }
    }
  }
  [u"element":u"ArrayReferenceProxy":private]=>
  &array(0) {
  }
}

Notice: Array to string conversion in %s on line %d
ArrayReferenceProxy::__construct(Array)

Notice: Array to string conversion in %s on line %d
ArrayReferenceProxy::offsetGet(Array, name)
NULL

Notice: Array to string conversion in %s on line %d
ArrayReferenceProxy::__construct(Array)

Notice: Array to string conversion in %s on line %d
ArrayReferenceProxy::offsetSet(Array, name, BlaBla)

Notice: Array to string conversion in %s on line %d
ArrayReferenceProxy::__construct(Array)

Notice: Array to string conversion in %s on line %d
ArrayReferenceProxy::offsetGet(Array, name)
unicode(6) "BlaBla"
===DONE===
