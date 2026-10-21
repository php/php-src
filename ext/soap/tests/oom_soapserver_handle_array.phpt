--TEST--
SoapServer::handle() doesn't reach hard limit through a SOAP-ENC array
--EXTENSIONS--
soap
--INI--
memory_limit=64M
--FILE--
<?php
function f() {}

$arrayType = 'xsd:string[' . implode(',', array_fill(0, 1000, 1)) . ']';
$items = str_repeat('<i/>', 1000);
$request = <<<XML
    <?xml version="1.0"?>
    <SOAP-ENV:Envelope
        xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"
        xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/"
        xmlns:xsd="http://www.w3.org/2001/XMLSchema"
        xmlns:ns1="http://test-uri/">
      <SOAP-ENV:Body><ns1:f><p SOAP-ENC:arrayType="$arrayType">$items</p></ns1:f></SOAP-ENV:Body>
    </SOAP-ENV:Envelope>
    XML;

$server = new SoapServer(null, ['uri' => 'http://test-uri/']);
$server->addFunction('f');
$server->handle($request);
?>
--EXPECTF--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"><SOAP-ENV:Body><SOAP-ENV:Fault><faultcode>SOAP-ENV:Server</faultcode><faultstring>Allowed memory size of 67108864 bytes exhausted %S(tried to allocate %d bytes)</faultstring></SOAP-ENV:Fault></SOAP-ENV:Body></SOAP-ENV:Envelope>
