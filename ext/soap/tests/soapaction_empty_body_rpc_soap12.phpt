--TEST--
SoapServer::handle() with empty SOAP 1.2 Body and SOAPAction matching an RPC operation with parameters
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--POST--
<env:Envelope xmlns:env="http://www.w3.org/2003/05/soap-envelope">
    <env:Body/>
</env:Envelope>
--FILE--
<?php
function Add($x, $y) {
    return $x + $y;
}

$server = new SoapServer(__DIR__.'/test.wsdl', ['soap_version' => SOAP_1_2]);
$server->addFunction("Add");
$_SERVER["HTTP_SOAPACTION"] = "Add";
$server->handle();
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<env:Envelope xmlns:env="http://www.w3.org/2003/05/soap-envelope"><env:Body><env:Fault><env:Code><env:Value>env:Sender</env:Value></env:Code><env:Reason><env:Text xml:lang="en">Missing parameter</env:Text></env:Reason></env:Fault></env:Body></env:Envelope>
