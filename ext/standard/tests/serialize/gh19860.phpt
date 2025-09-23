--TEST--
Bug GH-19860: Calling unserialize() inside Serializable::unserialize() allows the object to be manipulated before it has been restored.
--INI--
error_reporting=E_ALL & ~E_DEPRECATED
--FILE--
<?php

class Foo
{
    public function __construct(public string $val) {}

    public function __unserialize($data)
    {
        $this->val = $data['val'];
    }
}

class Bar implements Serializable
{
    public function __construct(public $obj) {}

    public function serialize()
    {
        return serialize($this->obj);
    }

    public function unserialize($data)
    {
        var_dump(unserialize($data)->val);
    }
}

$foo = new Foo('the value');
$bar = new Bar($foo);
$serialized = serialize($bar);
unserialize($serialized);
?>
--EXPECT--
string(9) "the value"
