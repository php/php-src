--TEST--
PEAR_Error: default error handling
--FILE--
<?php // -*- PHP -*-

// Test for: PEAR.php
// Parts tested: - PEAR_Error class
//               - PEAR::setErrorHandling
//               - PEAR::raiseError method

include dirname(__FILE__)."/../PEAR.php";

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

// Test 1
PEAR::setErrorHandling(PEAR_ERROR_PRINT, "OOPS: %s\n");
$tmp = new PEAR;
$tmp->raiseError("error happens");

// Return PEAR to its original state
$GLOBALS['_PEAR_default_error_mode']     = PEAR_ERROR_RETURN;
$GLOBALS['_PEAR_default_error_options']  = E_USER_NOTICE;
$GLOBALS['_PEAR_default_error_callback'] = '';

// Test 2
$obj = new PEAR;
$obj->setErrorHandling(PEAR_ERROR_PRINT);
$obj->raiseError("error 1\n");
$obj->setErrorHandling(null);
$obj->raiseError("error 2\n");
PEAR::setErrorHandling(PEAR_ERROR_CALLBACK, "errorhandler");
$obj->raiseError("error 3");
$obj->setErrorHandling(PEAR_ERROR_PRINT);
$obj->raiseError("error 4\n");

?>
--EXPECT--
OOPS: error happens
error 1
errorhandler called with an error object.
error message: error 3
error 4