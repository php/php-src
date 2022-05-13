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
        echo __METHOD__ . "(Array)\n";
        $this->object = $object;
        $this->element = &$element;
    }

    function offsetExists($index): bool {
        echo __METHOD__ . "($this->element, $index)\n";
        return array_key_exists($index, $this->element);
    }

    function offsetGet($index): mixed {
        echo __METHOD__ . "(Array, $index)\n";
        return isset($this->element[$index]) ? $this->element[$index] : NULL;
    }

    function offsetSet($index, $value): void {
        echo __METHOD__ . "(Array, $index, $value)\n";
        $this->element[$index] = $value;
    }

    function offsetUnset($index): void {
        echo __METHOD__ . "(Array, $index)\n";
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

    function offsetExists($index): bool
    {
        return array_key_exists($index, $this->person);
    }

    function offsetGet($index): mixed
    {
        return new ArrayReferenceProxy($this, $this->person[$index]);
    }

    function offsetSet($index, $value): void
    {
        $this->person[$index] = $value;
    }

    function offsetUnset($index): void
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
ArrayReferenceProxy::__construct(Array)
ArrayReferenceProxy::offsetGet(Array, name)
string(3) "Foo"
ArrayReferenceProxy::__construct(Array)
ArrayReferenceProxy::offsetSet(Array, name, FooBar)
ArrayReferenceProxy::__construct(Array)
ArrayReferenceProxy::offsetGet(Array, name)
string(6) "FooBar"
ArrayReferenceProxy::__construct(Array)
ArrayReferenceProxy::offsetSet(Array, name, FooBarBar)
ArrayReferenceProxy::__construct(Array)
ArrayReferenceProxy::offsetGet(Array, name)
string(9) "FooBarBar"
ArrayReferenceProxy::__construct(Array)
ArrayReferenceProxy::offsetGet(Array, name)
ArrayReferenceProxy::offsetSet(Array, name, FooBarBarBaz)
ArrayReferenceProxy::__construct(Array)
ArrayReferenceProxy::offsetGet(Array, name)
string(12) "FooBarBarBaz"
ArrayReferenceProxy::__construct(Array)
ArrayReferenceProxy::offsetUnset(Array, name)
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
ArrayReferenceProxy::__construct(Array)
ArrayReferenceProxy::offsetGet(Array, name)
NULL
ArrayReferenceProxy::__construct(Array)
ArrayReferenceProxy::offsetSet(Array, name, BlaBla)
ArrayReferenceProxy::__construct(Array)
ArrayReferenceProxy::offsetGet(Array, name)
string(6) "BlaBla"
