<?php // -*- C++ -*-

require_once "PEAR.php";

class __TestPEAR1 extends PEAR {
    function __TestPEAR1() {
	$this->_debug = true;
	$this->PEAR();
    }
}

print "test class __TestPEAR1\n";
$o = new __TestPEAR1;
var_dump(get_class($o));

?>
