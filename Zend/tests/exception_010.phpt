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
Warning: Throwable::getTraceAsString() expects exactly 0 parameters, 1 given in %s on line %d

Warning: Throwable::__toString() expects exactly 0 parameters, 1 given in %s on line %d

Warning: Throwable::getTrace() expects exactly 0 parameters, 1 given in %s on line %d

Warning: Throwable::getLine() expects exactly 0 parameters, 1 given in %s on line %d

Warning: Throwable::getFile() expects exactly 0 parameters, 1 given in %s on line %d

Warning: Throwable::getMessage() expects exactly 0 parameters, 1 given in %s on line %d

Warning: Throwable::getCode() expects exactly 0 parameters, 1 given in %s on line %d
