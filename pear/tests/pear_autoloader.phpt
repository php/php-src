--TEST--
PEAR_Autoloader
--SKIPIF--
skip
<?php /*if (!extension_loaded("overload")) die("skip\n"); */ ?>
--FILE--
<?php

include dirname(__FILE__)."/../PEAR/Autoloader.php";

class test1 extends PEAR_Autoloader {
    function test1() {
	$this->addAutoload(array(
	    'testfunc1' => 'testclass1',
	    'testfunca' => 'testclass1',
	    'testfunc2' => 'testclass2',
	    'testfuncb' => 'testclass2',
	));
    }
}

class testclass1 {
    function testfunc1($a) {
	print "testfunc1 arg=";var_dump($a);
	return 1;
    }
    function testfunca($a) {
	print "testfunca arg=";var_dump($a);
	return 2;
    }
}

class testclass2 {
    function testfunc2($b) {
	print "testfunc2 arg=";var_dump($b);
	return 3;
    }
    function testfuncb($b) {
	print "testfuncb arg=";var_dump($b);
	return 4;
    }
}

function dump($obj) {
    print "mapped methods:";
    foreach ($obj->_method_map as $method => $object) {
	print " $method";
    }
    print "\n";
}

function call($msg, $retval) {
    print "calling $msg returned $retval\n";
}

$obj = new test1;
dump($obj);
call("testfunc1", $obj->testfunc1(2));
dump($obj);
call("testfunca", $obj->testfunca(2));
dump($obj);
call("testfunc2", $obj->testfunc2(2));
dump($obj);
call("testfuncb", $obj->testfuncb(2));
dump($obj);

?>
--EXPECT--
mapped methods:
testfunc1 arg=int(2)
calling testfunc1 returned 1
mapped methods: testfunc1 testfunca
testfunca arg=int(2)
calling testfunca returned 2
mapped methods: testfunc1 testfunca
testfunc2 arg=int(2)
calling testfunc2 returned 3
mapped methods: testfunc1 testfunca testfunc2 testfuncb
testfuncb arg=int(2)
calling testfuncb returned 4
mapped methods: testfunc1 testfunca testfunc2 testfuncb
