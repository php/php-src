--TEST--
Bug #45282 (SoapClient has namespace issues when WSDL is distributed)
--EXTENSIONS--
soap
--SKIPIF--
<?php
if (getenv('SKIP_ASAN')) die('xleak libxml does not use the request allocator');
?>
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
try {
    new SoapClient(__DIR__ . "/bug45282_b.wsdl");
} catch (SoapFault $e) {
    echo $e->getMessage();
}
?>
--EXPECT--
SOAP-ERROR: Parsing WSDL: <message> 'reportHealthView' already defined
