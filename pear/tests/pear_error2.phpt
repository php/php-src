--TEST--
PEAR_Error: die mode
--SKIPIF--
<?php
if (!getenv('PHP_PEAR_RUNTESTS')) {
    echo 'skip';
}
?>
--FILE--
<?php // -*- C++ -*-

// Test for: PEAR.php
// Parts tested: - PEAR_Error class
//               - PEAR::isError static method
// testing PEAR_Error

include_once "PEAR.php";

error_reporting(E_ALL);

print "mode=die: ";
$err = new PEAR_Error("test error!!\n", -42, PEAR_ERROR_DIE);
print $err->toString() . "\n";

?>
--EXPECT--
mode=die: test error!!
