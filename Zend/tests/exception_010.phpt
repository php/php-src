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
Warning: Wrong parameter count for Exception::getTraceAsString() in %s on line %d

Warning: Wrong parameter count for Exception::__toString() in %s on line %d

Warning: Wrong parameter count for Exception::getTrace() in %s on line %d

Warning: Wrong parameter count for Exception::getLine() in %s on line %d

Warning: Wrong parameter count for Exception::getFile() in %s on line %d

Warning: Wrong parameter count for Exception::getMessage() in %s on line %d

Warning: Wrong parameter count for Exception::getCode() in %s on line %d
