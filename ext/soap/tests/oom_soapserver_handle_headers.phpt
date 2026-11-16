--TEST--
SoapServer::handle() doesn't reach hard limit through SOAP headers
--EXTENSIONS--
soap
--INI--
memory_limit=64M
--FILE--
<?php
function f() {}

$headers = str_repeat('<ns1:h/>', 20000);
$request = <<<XML
    <?xml version="1.0"?>
    <SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://test-uri/">
      <SOAP-ENV:Header>$headers</SOAP-ENV:Header>
      <SOAP-ENV:Body><ns1:f/></SOAP-ENV:Body>
    </SOAP-ENV:Envelope>
    XML;

$server = new SoapServer(null, ['uri' => 'http://test-uri/']);
$server->addFunction('f');
$server->handle($request);
?>
--EXPECTF--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"><SOAP-ENV:Body><SOAP-ENV:Fault><faultcode>SOAP-ENV:Server</faultcode><faultstring>Allowed memory size of 67108864 bytes exhausted %S(tried to allocate %d bytes)</faultstring></SOAP-ENV:Fault></SOAP-ENV:Body></SOAP-ENV:Envelope>
