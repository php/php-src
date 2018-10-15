--TEST--
Bug #65806 (unserialize fails with object which is referenced multiple times)
--FILE--
<?php
class myObjA {}
class myObjB {
    public $attrA;
    public $attrB;
}
class myObjC {
    public $attrC;
    public $attrD;
}
class myList {
    private $_serialized;
    private $_obj;

    public function __construct($obj)
    {
        $this->_obj = $obj;
        $this->_serialized = serialize($this->_obj);
    }
    public function get()
    {
        return $this->_obj;
    }
    public function __sleep()
    {
        $this->_serialized = serialize($this->_obj);
        return array(
            "\0" . __CLASS__ . "\0_serialized",
        );
    }
    public function __wakeup()
    {
		$this->_obj = unserialize($this->_serialized);
	}
}

echo "SCRIPT START" . PHP_EOL;

$objA = new myObjA();
$objB = new myObjB();
$objC = new myObjC();

$objB->attrA = new ArrayIterator();
$objB->attrB = $objA;

$objC->attrC = $objB;
$objC->attrD = $objA;

$list = new myList($objC);

echo 'check ' . check($list->get()) . PHP_EOL;

echo "start serialize/unserialize" . PHP_EOL;
$newList = unserialize(serialize($list));
echo "finish serialize/unserialize" . PHP_EOL;

//after unserialize the property myObjC::attrD is null instead of expected object
echo 'check ' . check($newList->get()) . PHP_EOL;

echo "SCRIPT END" . PHP_EOL ;

function check(myObjC $obj) {

    if (!is_object($obj->attrC)) {
        return 'failed (myObjC::attrC => ' . var_export($obj->attrC, true) . ')';
    }
    if (!is_object($obj->attrD)) {
        return 'failed (myObjC::attrD => ' . var_export($obj->attrD, true) . ')';
    }
    return 'successful';
}
?>
--EXPECT--
SCRIPT START
check successful
start serialize/unserialize
finish serialize/unserialize
check successful
SCRIPT END
