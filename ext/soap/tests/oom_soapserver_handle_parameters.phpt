--TEST--
SoapServer::handle() doesn't reach hard limit through request parameters
--EXTENSIONS--
soap
--INI--
memory_limit=64M
--FILE--
<?php
function f() {}

$params = str_repeat('<p><a/></p>', 400000);
$request = <<<XML
    <?xml version="1.0"?>
    <SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://test-uri/">
      <SOAP-ENV:Body><ns1:f>$params</ns1:f></SOAP-ENV:Body>
    </SOAP-ENV:Envelope>
    XML;

$server = new SoapServer(null, ['uri' => 'http://test-uri/']);
$server->addFunction('f');
$server->handle($request);
?>
--EXPECTF--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"><SOAP-ENV:Body><SOAP-ENV:Fault><faultcode>SOAP-ENV:Server</faultcode><faultstring>Allowed memory size of 67108864 bytes exhausted %S(tried to allocate %d bytes)</faultstring></SOAP-ENV:Fault></SOAP-ENV:Body></SOAP-ENV:Envelope>
