--TEST--
GH-23447 (Segfault when a class passed to SoapServer::setClass() fails to initialize)
--EXTENSIONS--
soap
--FILE--
<?php
class foo {
    private $broken = undefinedConstant;
}

$server = new SoapServer(null, array('uri' => 'http://testuri.org'));
$server->setClass('foo');

$server->handle(<<<'XML'
<?xml version="1.0"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/">
  <SOAP-ENV:Body><anything/></SOAP-ENV:Body>
</SOAP-ENV:Envelope>
XML);

echo "ok\n";
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"><SOAP-ENV:Body><SOAP-ENV:Fault><faultcode>SOAP-ENV:Server</faultcode><faultstring>Undefined constant "undefinedConstant"</faultstring></SOAP-ENV:Fault></SOAP-ENV:Body></SOAP-ENV:Envelope>
ok
