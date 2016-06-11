--TEST--
Bug #71839: Deserializing serialized SPLObjectStorage-Object can't access properties in PHP
--FILE--
<?php

class A extends SplObjectStorage {
    protected $a = null;

    public function __construct() {
        $this->a = '123';
    }
    
    public function getA() {
        return $this->a;
    }
}

$serialized = serialize(new A());
$obj = unserialize($serialized);
var_dump($obj->getA());

?>
--EXPECT--
string(3) "123"
