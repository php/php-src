--TEST--
PEAR constructor/destructor test
--SKIPIF--
--FILE--
<?php

require_once "PEAR.php";

class TestPEAR extends PEAR {
    function TestPEAR($name) {
	$this->_debug = true;
	$this->name = $name;
	$this->PEAR();
    }
    function _TestPEAR() {
	print "This is the TestPEAR($this->name) destructor\n";
	$this->_PEAR();
    }
}

print "test class TestPEAR\n";
$o = new TestPEAR("test1");
$p = new TestPEAR("test2");
var_dump(get_class($o));
var_dump(get_class($p));

?>
--GET--
--POST--
--EXPECT--
test class TestPEAR
PEAR constructor called, class=testpear
PEAR constructor called, class=testpear
string(8) "testpear"
string(8) "testpear"
This is the TestPEAR(test1) destructor
PEAR destructor called, class=testpear
This is the TestPEAR(test2) destructor
PEAR destructor called, class=testpear
