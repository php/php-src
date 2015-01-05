--TEST--
Testing Exception's methods
--FILE--
<?php

$x = new Exception;
$x->gettraceasstring(1);
$x->gettraceasstring();
$x->__tostring(1);
$x->gettrace(1);
$x->getline(1);
$x->getfile(1);
$x->getmessage(1);
$x->getcode(1);

?>
--EXPECTF--
Warning: Exception::getTraceAsString() expects exactly 0 parameters, 1 given in %s on line %d

Warning: Exception::__toString() expects exactly 0 parameters, 1 given in %s on line %d

Warning: Exception::getTrace() expects exactly 0 parameters, 1 given in %s on line %d

Warning: Exception::getLine() expects exactly 0 parameters, 1 given in %s on line %d

Warning: Exception::getFile() expects exactly 0 parameters, 1 given in %s on line %d

Warning: Exception::getMessage() expects exactly 0 parameters, 1 given in %s on line %d

Warning: Exception::getCode() expects exactly 0 parameters, 1 given in %s on line %d
