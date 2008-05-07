--TEST--
Bug #44811 (Improve error messages when creating new SoapClient which contains invalid data)
--SKIPIF--
<?php require_once 'skipif.inc'; ?>
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
try {
    $x = new SoapClient('http://slashdot.org');
} catch (SoapFault $e) {
    echo $e->getMessage() . PHP_EOL;
}
die('ok');
?>
--EXPECT--
SOAP-ERROR: Parsing WSDL: Couldn't load from 'http://slashdot.org' : Premature end of data in tag html line 3

ok
