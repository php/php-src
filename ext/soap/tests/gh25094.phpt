--TEST--
GH-25094 (NULL pointer dereference in get_doc_function() when the request has fewer parts than the WSDL)
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
class TestService {
    public function Test($a, $b) {
        return $a;
    }
}

$server = new SoapServer(__DIR__ . '/gh25094.wsdl');
$server->setClass('TestService');

$server->handle(<<<'XML'
<?xml version="1.0"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:tns="http://test-uri">
  <SOAP-ENV:Body><tns:a>foo</tns:a></SOAP-ENV:Body>
</SOAP-ENV:Envelope>
XML);
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"><SOAP-ENV:Body><SOAP-ENV:Fault><faultcode>SOAP-ENV:Server</faultcode><faultstring>Procedure 'a' not present</faultstring></SOAP-ENV:Fault></SOAP-ENV:Body></SOAP-ENV:Envelope>
