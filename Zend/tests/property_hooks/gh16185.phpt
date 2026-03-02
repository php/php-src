--TEST--
GH-16185: Incorrect indexing into dynamic property array
--FILE--
<?php

class ByRef {
    private $_virtualByRef = 'virtualByRef';
}

class ByVal extends ByRef {
    public $_virtualByRef {
        get => null;
        set { $this->dynamicProp = $value; }
    }
}

$object = new ByVal;
foreach ($object as $value) {
    var_dump($value);
    $object->_virtualByRef = $value;
}

?>
--EXPECTF--
NULL

Deprecated: Creation of dynamic property ByVal::$dynamicProp is deprecated in %s on line %d
NULL
