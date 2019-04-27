--TEST--
SPL: Bug #66834
--FILE--
<?php

// overrides both offsetExists and offsetGet
class ArrayObjectBoth extends ArrayObject
{
	public function offsetExists($offset) {
		var_dump('Called: '.__METHOD__);
		return parent::offsetExists($offset);
	}

	public function offsetGet($offset) {
		var_dump('Called: '.__METHOD__);
		return parent::offsetGet($offset);
	}
}

// overrides only offsetExists
class ArrayObjectExists extends ArrayObject
{
	public function offsetExists($offset) {
		var_dump('Called: '.__METHOD__);
		return parent::offsetExists($offset);
	}
}

// overrides only offsetGet
class ArrayObjectGet extends ArrayObject
{
	public function offsetGet($offset) {
		var_dump('Called: '.__METHOD__);
		return parent::offsetGet($offset);
	}
}

// overrides only offsetGet and offsetSet
class ArrayObjectGetSet extends ArrayObject
{
	public function offsetGet($offset)
	{
		return parent::offsetGet(str_rot13($offset));
	}

	public function offsetSet($offset, $value)
	{
		return parent::offsetSet(str_rot13($offset), $value);
	}
}

$values = ['foo' => '', 'bar' => null, 'baz' => 42];

echo "==== class with offsetExists() and offsetGet() ====\n";
$object = new ArrayObjectBoth($values);
var_dump($object->offsetExists('foo'), isset($object['foo']), empty($object['foo']));
var_dump($object->offsetExists('bar'), isset($object['bar']), empty($object['bar']));
var_dump($object->offsetexists('baz'), isset($object['baz']), empty($object['baz']));
var_dump($object->offsetexists('qux'), isset($object['qux']), empty($object['qux']));

echo "==== class with offsetExists() ====\n";
$object = new ArrayObjectExists($values);
var_dump($object->offsetExists('foo'), isset($object['foo']), empty($object['foo']));
var_dump($object->offsetExists('bar'), isset($object['bar']), empty($object['bar']));
var_dump($object->offsetexists('baz'), isset($object['baz']), empty($object['baz']));
var_dump($object->offsetexists('qux'), isset($object['qux']), empty($object['qux']));

echo "==== class with offsetGet() ====\n";
$object = new ArrayObjectGet($values);
var_dump($object->offsetExists('foo'), isset($object['foo']), empty($object['foo']));
var_dump($object->offsetExists('bar'), isset($object['bar']), empty($object['bar']));
var_dump($object->offsetexists('baz'), isset($object['baz']), empty($object['baz']));
var_dump($object->offsetexists('qux'), isset($object['qux']), empty($object['qux']));

echo "==== class with offsetGet() and offsetSet() ====\n";
$object = new ArrayObjectGetSet;
$object['foo'] = 42;
var_dump($object->offsetExists('foo'), $object->offsetExists('sbb'), isset($object['foo']), isset($object['sbb']), empty($object['sbb']));

?>
--EXPECTF--
==== class with offsetExists() and offsetGet() ====
string(37) "Called: ArrayObjectBoth::offsetExists"
string(37) "Called: ArrayObjectBoth::offsetExists"
string(37) "Called: ArrayObjectBoth::offsetExists"
string(34) "Called: ArrayObjectBoth::offsetGet"
bool(true)
bool(true)
bool(true)
string(37) "Called: ArrayObjectBoth::offsetExists"
string(37) "Called: ArrayObjectBoth::offsetExists"
string(37) "Called: ArrayObjectBoth::offsetExists"
string(34) "Called: ArrayObjectBoth::offsetGet"
bool(true)
bool(true)
bool(true)
string(37) "Called: ArrayObjectBoth::offsetExists"
string(37) "Called: ArrayObjectBoth::offsetExists"
string(37) "Called: ArrayObjectBoth::offsetExists"
string(34) "Called: ArrayObjectBoth::offsetGet"
bool(true)
bool(true)
bool(false)
string(37) "Called: ArrayObjectBoth::offsetExists"
string(37) "Called: ArrayObjectBoth::offsetExists"
string(37) "Called: ArrayObjectBoth::offsetExists"
bool(false)
bool(false)
bool(true)
==== class with offsetExists() ====
string(39) "Called: ArrayObjectExists::offsetExists"
string(39) "Called: ArrayObjectExists::offsetExists"
string(39) "Called: ArrayObjectExists::offsetExists"
bool(true)
bool(true)
bool(true)
string(39) "Called: ArrayObjectExists::offsetExists"
string(39) "Called: ArrayObjectExists::offsetExists"
string(39) "Called: ArrayObjectExists::offsetExists"
bool(true)
bool(true)
bool(true)
string(39) "Called: ArrayObjectExists::offsetExists"
string(39) "Called: ArrayObjectExists::offsetExists"
string(39) "Called: ArrayObjectExists::offsetExists"
bool(true)
bool(true)
bool(false)
string(39) "Called: ArrayObjectExists::offsetExists"
string(39) "Called: ArrayObjectExists::offsetExists"
string(39) "Called: ArrayObjectExists::offsetExists"
bool(false)
bool(false)
bool(true)
==== class with offsetGet() ====
string(33) "Called: ArrayObjectGet::offsetGet"
bool(true)
bool(true)
bool(true)
string(33) "Called: ArrayObjectGet::offsetGet"
bool(true)
bool(false)
bool(true)
string(33) "Called: ArrayObjectGet::offsetGet"
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
bool(true)
==== class with offsetGet() and offsetSet() ====

Notice: Undefined index: foo in %s on line %d
bool(false)
bool(true)
bool(false)
bool(true)
bool(true)
