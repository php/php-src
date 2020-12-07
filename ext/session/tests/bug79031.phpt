--TEST--
Bug #79031: Session unserialization problem
--FILE--
<?php

class SerializableClass implements Serializable {
    public $sharedProp;
    public function __construct($prop)
    {
        $this->sharedProp = $prop;
    }
    public function __set($key, $value)
    {
        $this->$key = $value;
    }
    public function serialize()
    {
        return serialize(get_object_vars($this));
    }
    public function unserialize($data)
    {
        $ar = unserialize($data);
        if ($ar === false) {
            return;
        }
        foreach ($ar as $k => $v) {
            $this->__set($k, $v);
        }
    }
}

// Shared object that acts as property of two another objects stored in session
$testPropertyObj = new stdClass();
$testPropertyObj->name = 'test';

// Two instances of \SerializableClass that shares property
$sessionObject = [
    'obj1' => new SerializableClass($testPropertyObj),
    'obj2' => new SerializableClass($testPropertyObj),
];
session_start();
$_SESSION = $sessionObject;

$sessionString = session_encode();
session_decode($sessionString);
echo $sessionString;
echo "\n\n";
var_dump($_SESSION);

?>
--EXPECTF--
Deprecated: The Serializable interface is deprecated. Implement __serialize() and __unserialize() instead (or in addition, if support for old PHP versions is necessary) in %s on line %d

Warning: session_start(): Session cannot be started after headers have already been sent in %s on line %d

Warning: session_encode(): Cannot encode non-existent session in %s on line %d

Warning: session_decode(): Session data cannot be decoded when there is no active session in %s on line %d


array(2) {
  ["obj1"]=>
  object(SerializableClass)#2 (1) {
    ["sharedProp"]=>
    object(stdClass)#1 (1) {
      ["name"]=>
      string(4) "test"
    }
  }
  ["obj2"]=>
  object(SerializableClass)#3 (1) {
    ["sharedProp"]=>
    object(stdClass)#1 (1) {
      ["name"]=>
      string(4) "test"
    }
  }
}
