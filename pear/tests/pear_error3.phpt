--TEST--
PEAR_Error: default error handling
--FILE--
<?php // -*- PHP -*-

// Test for: PEAR.php
// Parts tested: - PEAR_Error class
//               - PEAR::setErrorHandling
//               - PEAR::raiseError method

require "../PEAR.php";

error_reporting(E_ALL);

function errorhandler($eobj)
{
    if (PEAR::isError($eobj)) {
        print "errorhandler called with an error object.\n";
        print "error message: ".$eobj->getMessage()."\n";
    } else {
        print "errorhandler called, but without an error object.\n";
    }
}

$obj = new PEAR;
$obj->setErrorHandling(PEAR_ERROR_PRINT);
$obj->raiseError("error 1\n");
$obj->setErrorHandling(null);
$obj->raiseError("error 2\n");
PEAR::setErrorHandling(PEAR_ERROR_CALLBACK, "errorhandler");
$obj->raiseError("error 3\n");
$obj->setErrorHandling(PEAR_ERROR_PRINT);
$obj->raiseError("error 4\n");

?>
--EXPECT--
error 1
errorhandler called with an error object.
error message: error 3
error 4
