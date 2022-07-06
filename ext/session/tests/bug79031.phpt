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
--EXPECT--
obj1|C:17:"SerializableClass":65:{a:1:{s:10:"sharedProp";O:8:"stdClass":1:{s:4:"name";s:4:"test";}}}obj2|C:17:"SerializableClass":28:{a:1:{s:10:"sharedProp";r:3;}}

array(2) {
  ["obj1"]=>
  object(SerializableClass)#4 (1) {
    ["sharedProp"]=>
    object(stdClass)#5 (1) {
      ["name"]=>
      string(4) "test"
    }
  }
  ["obj2"]=>
  object(SerializableClass)#6 (1) {
    ["sharedProp"]=>
    object(stdClass)#5 (1) {
      ["name"]=>
      string(4) "test"
    }
  }
}
