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

// test for bug http://bugs.php.net/bug.php?id=14744
class Other extends Pear {

    var $a = 'default value';

    function Other() {
        $this->PEAR();
    }

    function _Other() {
        // $a was modified but here misteriously returns to be
        // the original value. That makes the destructor useless
        // The correct value for $a in the destructor shoud be "new value"
        echo "#bug 14744# Other class destructor: other->a == '" . $this->a ."'\n";
    }
}

print "testing plain destructors\n";
$o = new TestPEAR("test1");
$p = new TestPEAR("test2");
print "..\n";
print "testing inherited destructors\n";
$q = new Test3;

echo "...\ntesting bug #14744\n";
$other =& new Other;
echo "#bug 14744# Other class constructor: other->a == '" . $other->a ."'\n";
// Modify $a
$other->a = 'new value';
echo "#bug 14744# Other class modified: other->a == '" . $other->a ."'\n";

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
...
testing bug #14744
#bug 14744# Other class constructor: other->a == 'default value'
#bug 14744# Other class modified: other->a == 'new value'
..
script exiting...
..
This is the TestPEAR(test1) destructor
PEAR destructor called, class=testpear
This is the TestPEAR(test2) destructor
PEAR destructor called, class=testpear
This is the Test2 destructor
#bug 14744# Other class destructor: other->a == 'new value'
