--TEST--
SPL: Bug #66834
--FILE--
<?php

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

echo "==== class with offsetExists() and offsetGet() ====\n";
$object = new ArrayObjectBoth(array('foo' => ''));
var_dump($object->offsetExists('foo'), isset($object['foo']), empty($object['foo']));

class ArrayObjectExists extends ArrayObject {
  public function offsetExists($offset) {
    var_dump('Called: '.__METHOD__);
    return parent::offsetExists($offset);
  }
}

echo "==== class with offsetExists() ====\n";
$object = new ArrayObjectExists(array('foo' => ''));
var_dump($object->offsetExists('foo'), isset($object['foo']), empty($object['foo']));

class ArrayObjectGet extends ArrayObject {
  public function offsetGet($offset) {
    var_dump('Called: '.__METHOD__);
    return parent::offsetGet($offset);
  }
}

echo "==== class with offsetGet() ====\n";
$object = new ArrayObjectGet(array('foo' => ''));
var_dump($object->offsetExists('foo'), isset($object['foo']), empty($object['foo']));

?>
--EXPECT--
==== class with offsetExists() and offsetGet() ====
string(37) "Called: ArrayObjectBoth::offsetExists"
string(37) "Called: ArrayObjectBoth::offsetExists"
string(34) "Called: ArrayObjectBoth::offsetGet"
string(37) "Called: ArrayObjectBoth::offsetExists"
string(34) "Called: ArrayObjectBoth::offsetGet"
bool(true)
bool(true)
bool(true)
==== class with offsetExists() ====
string(39) "Called: ArrayObjectExists::offsetExists"
string(39) "Called: ArrayObjectExists::offsetExists"
string(39) "Called: ArrayObjectExists::offsetExists"
bool(true)
bool(true)
bool(true)
==== class with offsetGet() ====
string(33) "Called: ArrayObjectGet::offsetGet"
string(33) "Called: ArrayObjectGet::offsetGet"
bool(true)
bool(true)
bool(true)
