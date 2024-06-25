--TEST--
GH-14586: SoapFault::__construct() leaks memory if called twice
--EXTENSIONS--
soap
--FILE--
<?php
$sf = new SoapFault(null, "x");
$sf->__construct(null, "x");
?>
DONE
--EXPECT--
DONE
