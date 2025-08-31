--TEST--
ZE2 Serializable
--FILE--
<?php

class Test implements Serializable
{
    public $data;

    function __construct($data)
    {
        echo __METHOD__ . "($data)\n";
        $this->data = $data;
    }

    function serialize()
    {
        echo __METHOD__ . "({$this->data})\n";
        return $this->data;
    }

    function unserialize($serialized)
    {
        echo __METHOD__ . "($serialized)\n";
        $this->data = $serialized;
        var_dump($this);
    }
}

$tests = array('String', NULL, 42, false);

foreach($tests as $data)
{
    try
    {
        echo "==========\n";
        var_dump($data);
        $ser = serialize(new Test($data));
        var_dump(unserialize($ser));
    }
    catch(Exception $e)
    {
        echo 'Exception: ' . $e->getMessage() . "\n";
    }
}

?>
--EXPECTF--
Deprecated: %s implements the Serializable interface, which is deprecated. Implement __serialize() and __unserialize() instead (or in addition, if support for old PHP versions is necessary) in %s on line %d
==========
string(6) "String"
Test::__construct(String)
Test::serialize(String)
Test::unserialize(String)
object(Test)#%d (1) {
  ["data"]=>
  string(6) "String"
}
object(Test)#%d (1) {
  ["data"]=>
  string(6) "String"
}
==========
NULL
Test::__construct()
Test::serialize()
NULL
==========
int(42)
Test::__construct(42)
Test::serialize(42)
Exception: Test::serialize() must return a string or NULL
==========
bool(false)
Test::__construct()
Test::serialize()
Exception: Test::serialize() must return a string or NULL
