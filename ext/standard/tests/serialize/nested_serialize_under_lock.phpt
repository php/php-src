--TEST--
Check that nested serialization/unserialization still works correctly while under lock
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

// Going through spl_autoload_register() to lock serialization
spl_autoload_register(function($class) {
    $testPropertyObj = new stdClass();
    $testPropertyObj->name = 'test';
    $array = [
        'obj1' => new SerializableClass($testPropertyObj),
        'obj2' => new SerializableClass($testPropertyObj),
    ];
    var_dump(unserialize(serialize($array)));
    class X {}
});

unserialize('O:1:"X":0:{}');
?>
--EXPECT--
array(2) {
  ["obj1"]=>
  object(SerializableClass)#5 (1) {
    ["sharedProp"]=>
    object(stdClass)#6 (1) {
      ["name"]=>
      string(4) "test"
    }
  }
  ["obj2"]=>
  object(SerializableClass)#7 (1) {
    ["sharedProp"]=>
    object(stdClass)#6 (1) {
      ["name"]=>
      string(4) "test"
    }
  }
}
