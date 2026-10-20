--TEST--
Bug #62672 (Error on serialize of ArrayObject)
--FILE--
<?php

class ObjA
{
    private $_varA;

    public function __construct(Iterator $source)
    {
        $this->_varA = $source;
    }
}

class ObjB extends ObjA
{
    private $_varB;

    public function __construct(ArrayObject $keys)
    {
        $this->_varB = $keys;
        parent::__construct($keys->getIterator());
    }
}

$obj = new ObjB(new ArrayObject());

var_dump($obj == unserialize(serialize($obj)));
?>
--EXPECTF--
Deprecated: ArrayIterator::__unserialize(): Using an object as a backing array for ArrayIterator is deprecated, as it allows violating class constraints and invariants in %s on line %d
bool(true)
