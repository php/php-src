--TEST--
PEAR_Error in die mode
--SKIPIF--
--FILE--
<?php // -*- C++ -*-

// Test for: PEAR.php
// Parts tested: - PEAR_Error class
//               - PEAR::isError static method
// testing PEAR_Error

require_once "PEAR.php";

error_reporting(4095);

print "mode=die: ";
$err = new PEAR_Error("test error!!\n", -42, PEAR_ERROR_DIE);
print $err->toString() . "\n";

?>
--GET--
--POST--
--EXPECT--
mode=die: test error!!
