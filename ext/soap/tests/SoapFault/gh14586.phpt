--TEST--
GH-14586: SoapFault::__construct() leaks memory if called twice
--EXTENSIONS--
soap
--FILE--
<?php
$sf = new SoapFault(null, "x");
$sf->__construct(null, "x");
try {
    $sf->__construct("", "");
} catch (ValueError) {}
$sf->__construct(null, "x", headerFault: []);
var_dump($sf->headerfault);
$sf->__construct(null, "x");
var_dump($sf->headerfault);
?>
DONE
--EXPECT--
array(0) {
}
NULL
DONE
