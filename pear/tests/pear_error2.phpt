--TEST--
PEAR_Error: die mode
--SKIPIF--
--FILE--
<?php // -*- C++ -*-

// Test for: PEAR.php
// Parts tested: - PEAR_Error class
//               - PEAR::isError static method
// testing PEAR_Error

include dirname(__FILE__)."/../PEAR.php";

error_reporting(E_ALL);

print "mode=die: ";
$err = new PEAR_Error("test error!!\n", -42, PEAR_ERROR_DIE);
print $err->toString() . "\n";

?>
--GET--
--POST--
--EXPECT--
mode=die: test error!!
