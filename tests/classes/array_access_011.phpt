--TEST--
ZE2 ArrayAccess and ArrayAccessReferenceProxy with references to main array
--FILE--
<?php

// NOTE: This will become part of SPL

class ArrayAccessReferenceProxy implements ArrayAccess
{
    private $object;
    private $oarray;
    private $element;

    function __construct(ArrayAccess $object, array &$array, $element)
    {
        echo __METHOD__ . "($element)\n";
        $this->object = $object;
        $this->oarray = &$array;
        $this->element = $element;
    }

    function offsetExists($index) {
        echo __METHOD__ . "($this->element, $index)\n";
        return array_key_exists($index, $this->oarray[$this->element]);
    }

    function offsetGet($index) {
        echo __METHOD__ . "($this->element, $index)\n";
        return isset($this->oarray[$this->element][$index]) ? $this->oarray[$this->element][$index] : NULL;
    }

    function offsetSet($index, $value) {
        echo __METHOD__ . "($this->element, $index, $value)\n";
        $this->oarray[$this->element][$index] = $value;
    }

    function offsetUnset($index) {
        echo __METHOD__ . "($this->element, $index)\n";
        unset($this->oarray[$this->element][$index]);
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
        if (is_array($this->person[$index]))
        {
            return new ArrayAccessReferenceProxy($this, $this->person, $index);
        }
        else
        {
            return $this->person[$index];
        }
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
--EXPECTF--
string(3) "Foo"
string(6) "FooBar"
string(9) "FooBarBaz"
===ArrayOverloading===
ArrayAccessReferenceProxy::__construct(0)
object(ArrayAccessReferenceProxy)#%d (3) {
  ["object":"ArrayAccessReferenceProxy":private]=>
  object(Peoples)#%d (1) {
    ["person"]=>
    &array(1) {
      [0]=>
      array(1) {
        ["name"]=>
        string(3) "Foo"
      }
    }
  }
  ["oarray":"ArrayAccessReferenceProxy":private]=>
  &array(1) {
    [0]=>
    array(1) {
      ["name"]=>
      string(3) "Foo"
    }
  }
  ["element":"ArrayAccessReferenceProxy":private]=>
  int(0)
}
ArrayAccessReferenceProxy::__construct(0)
ArrayAccessReferenceProxy::offsetGet(0, name)
string(3) "Foo"
ArrayAccessReferenceProxy::__construct(0)
ArrayAccessReferenceProxy::offsetSet(0, name, FooBar)
ArrayAccessReferenceProxy::__construct(0)
ArrayAccessReferenceProxy::offsetGet(0, name)
string(6) "FooBar"
ArrayAccessReferenceProxy::__construct(0)
ArrayAccessReferenceProxy::offsetSet(0, name, FooBarBar)
ArrayAccessReferenceProxy::__construct(0)
ArrayAccessReferenceProxy::offsetGet(0, name)
string(9) "FooBarBar"
ArrayAccessReferenceProxy::__construct(0)
ArrayAccessReferenceProxy::offsetGet(0, name)
ArrayAccessReferenceProxy::offsetSet(0, name, FooBarBarBaz)
ArrayAccessReferenceProxy::__construct(0)
ArrayAccessReferenceProxy::offsetGet(0, name)
string(12) "FooBarBarBaz"
ArrayAccessReferenceProxy::__construct(0)
ArrayAccessReferenceProxy::offsetUnset(0, name)
ArrayAccessReferenceProxy::__construct(0)
object(ArrayAccessReferenceProxy)#%d (3) {
  ["object":"ArrayAccessReferenceProxy":private]=>
  object(Peoples)#%d (1) {
    ["person"]=>
    &array(1) {
      [0]=>
      array(0) {
      }
    }
  }
  ["oarray":"ArrayAccessReferenceProxy":private]=>
  &array(1) {
    [0]=>
    array(0) {
    }
  }
  ["element":"ArrayAccessReferenceProxy":private]=>
  int(0)
}
ArrayAccessReferenceProxy::__construct(0)
ArrayAccessReferenceProxy::offsetGet(0, name)
NULL
ArrayAccessReferenceProxy::__construct(0)
ArrayAccessReferenceProxy::offsetSet(0, name, BlaBla)
ArrayAccessReferenceProxy::__construct(0)
ArrayAccessReferenceProxy::offsetGet(0, name)
string(6) "BlaBla"
