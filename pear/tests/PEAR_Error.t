<?php // -*- C++ -*-

// Test for: PEAR.php
// Parts tested: - PEAR_Error class
//               - PEAR::isError static method
// testing PEAR_Error

require_once "PEAR.php";

print "new PEAR_Error ";
var_dump($err = new PEAR_Error);
print "isError 1 ";
var_dump(PEAR::isError($err));
print "isError 2 ";
$str = "not an error";
var_dump(PEAR::isError($str));

?>
