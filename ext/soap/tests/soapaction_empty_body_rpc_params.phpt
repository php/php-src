--TEST--
SoapServer::handle() with empty Body and SOAPAction matching an RPC operation with parameters
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--POST--
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/">
    <SOAP-ENV:Body/>
</SOAP-ENV:Envelope>
--FILE--
<?php
function Add($x, $y) {
    return $x + $y;
}

$server = new SoapServer(__DIR__.'/test.wsdl');
$server->addFunction("Add");
$_SERVER["HTTP_SOAPACTION"] = "Add";
$server->handle();
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"><SOAP-ENV:Body><SOAP-ENV:Fault><faultcode>SOAP-ENV:Client</faultcode><faultstring>Missing parameter</faultstring></SOAP-ENV:Fault></SOAP-ENV:Body></SOAP-ENV:Envelope>
