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

class Test2 extends PEAR {
    function _Test2() {
        print "This is the Test2 destructor\n";
	$this->_PEAR();
    }
}

class Test3 extends Test2 {
}

print "testing plain destructors\n";
$o = new TestPEAR("test1");
$p = new TestPEAR("test2");
print "..\n";
print "testing inherited destructors\n";
$q = new Test3;

print "..\n";
print "script exiting...\n";
print "..\n";

?>
--GET--
--POST--
--EXPECT--
testing plain destructors
PEAR constructor called, class=testpear
PEAR constructor called, class=testpear
..
testing inherited destructors
..
script exiting...
..
This is the TestPEAR(test1) destructor
PEAR destructor called, class=testpear
This is the TestPEAR(test2) destructor
PEAR destructor called, class=testpear
This is the Test2 destructor
